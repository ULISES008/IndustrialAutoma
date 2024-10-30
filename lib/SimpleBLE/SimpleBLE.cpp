#include "SimpleBLE.h"

uint8_t SimpleBLE::own_addr_type;
uint16_t SimpleBLE::gatt_read_val_handle;
uint16_t SimpleBLE::gatt_write_val_handle;
QueueHandle_t SimpleBLE::uart_queue = NULL;
uint16_t SimpleBLE::conn_handle;
bool SimpleBLE::_data_available = false;
char SimpleBLE::_message_in[128] = {0};
char SimpleBLE::_message_out[128] = {0};
uint16_t SimpleBLE::_length_in = 0;
size_t SimpleBLE::_length_out = 0;

const struct ble_gatt_svc_def SimpleBLE::gatt_svr_defs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = &write_uuid.u,
                .access_cb = gattHandler,
                .flags = BLE_GATT_CHR_F_WRITE,
                .val_handle = &gatt_write_val_handle,
            },
            {
                .uuid = &read_uuid.u,
                .access_cb = gattHandler,
                .flags = BLE_GATT_CHR_F_READ|BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &gatt_read_val_handle,
            },
            {0}},
    },
    {
        0,
    },
};

void SimpleBLE::begin(const char *deviceName)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE("BLE", "Failed to init nimble %d", ret);
        return;
    }

    int rc = ble_svc_gap_device_name_set(deviceName);
    assert(rc == 0);

    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_gatts_count_cfg(gatt_svr_defs);
    ble_gatts_add_svcs(gatt_svr_defs);
    
    ble_store_config_init();
    ble_hs_cfg.reset_cb = bleOnReset;
    ble_hs_cfg.sync_cb = bleOnSync;

    nimble_port_freertos_init(bleHostTask);
}

int SimpleBLE::gattHandler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch (ctxt->op)
    {
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        ble_hs_mbuf_to_flat(ctxt->om, _message_in, sizeof(_message_in), &_length_in);
        _message_in[_length_in] = '\0';
        _data_available = true;
        break;
    case BLE_GATT_ACCESS_OP_READ_CHR:
        os_mbuf_append(ctxt->om, _message_out, _length_out);
        break;
    default:
        break;
    }
    return 0;
}

bool SimpleBLE::available()
{
    return _data_available;
}

int SimpleBLE::read(char buffer[], uint32_t length)
{
    memcpy(buffer, _message_in, _length_in);
    _data_available = false;
    return _length_in;
}

void SimpleBLE::write(char data[], size_t length)
{
    struct os_mbuf *txom = ble_hs_mbuf_from_flat(data, length);
    int rc = ble_gatts_notify_custom(conn_handle, gatt_read_val_handle, txom);
    if (rc != 0)
    {
        ESP_LOGI("BLE", "Error in sending notification rc = %d", rc);
    }
    _length_out = length;
    memcpy(_message_out, data, length);
}

int SimpleBLE::bleGapEvent(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("BLE", "connection %s; status=%d",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);
        if (event->connect.status == 0)
        {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            printConnDesc(&desc);
        }
        if (event->connect.status != 0 || CONFIG_BT_NIMBLE_MAX_CONNECTIONS > 2)
        {
            bleAdvertise();
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("BLE", "disconnect; reason=%d", event->disconnect.reason);
        printConnDesc(&event->disconnect.conn);
        conn_handle = 0;
        bleAdvertise();
        break;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI("BLE", "connection updated; status=%d", event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        printConnDesc(&desc);
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("BLE", "advertise complete; reason=%d", event->adv_complete.reason);
        bleAdvertise();
        break;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI("BLE", "mtu update event; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI("BLE", "subscribe event; conn_handle=%d attr_handle=%d reason=%d prevn=%d curn=%d previ=%d curi=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle, event->subscribe.reason,
                 event->subscribe.prev_notify, event->subscribe.cur_notify,
                 event->subscribe.prev_indicate, event->subscribe.cur_indicate);
        conn_handle = 1;
        break;

    default:
        return 0;
    }
    return 0;
}

void SimpleBLE::bleAdvertise()
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;
    int rc;

    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    static const ble_uuid16_t service_uuid = svc_uuid;
    fields.uuids16 = &service_uuid;
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        ESP_LOGE("BLE", "error setting advertisement data; rc=%d", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    if (ble_gap_adv_active())
    {
        ESP_LOGI("BLE", "Advertising already active, stopping and restarting");
        rc = ble_gap_adv_stop();
        if (rc != 0)
        {
            ESP_LOGE("BLE", "error stopping advertisement; rc=%d", rc);
            return;
        }
    }

    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params, bleGapEvent, NULL);
    if (rc != 0)
    {
        ESP_LOGE("BLE", "error enabling advertisement; rc=%d", rc);
        return;
    }
}

void SimpleBLE::bleOnSync()
{
    int rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0)
    {
        ESP_LOGE("BLE", "error determining address type; rc=%d", rc);
        return;
    }

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    ESP_LOGI("BLE", "Device Address: %s", addr_val);
    bleAdvertise();
}

void SimpleBLE::bleOnReset(int reason)
{
    ESP_LOGE("BLE", "Resetting state; reason=%d", reason);
}

void SimpleBLE::bleHostTask(void *param)
{
    ESP_LOGI("BLE", "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void SimpleBLE::printConnDesc(struct ble_gap_conn_desc *desc)
{
    ESP_LOGI("BLE", "handle=%d our_ota_addr_type=%d our_ota_addr=", desc->conn_handle, desc->our_ota_addr.type);
    ESP_LOG_BUFFER_HEX("BLE", desc->our_ota_addr.val, 6);
    ESP_LOGI("BLE", " our_id_addr_type=%d our_id_addr=", desc->our_id_addr.type);
    ESP_LOG_BUFFER_HEX("BLE", desc->our_id_addr.val, 6);
    ESP_LOGI("BLE", " peer_ota_addr_type=%d peer_ota_addr=", desc->peer_ota_addr.type);
    ESP_LOG_BUFFER_HEX("BLE", desc->peer_ota_addr.val, 6);
    ESP_LOGI("BLE", " peer_id_addr_type=%d peer_id_addr=", desc->peer_id_addr.type);
    ESP_LOG_BUFFER_HEX("BLE", desc->peer_id_addr.val, 6);
    ESP_LOGI("BLE", " conn_itvl=%d conn_latency=%d supervision_timeout=%d encrypted=%d authenticated=%d bonded=%d",
             desc->conn_itvl, desc->conn_latency, desc->supervision_timeout, desc->sec_state.encrypted, desc->sec_state.authenticated, desc->sec_state.bonded);
}

void SimpleBLE::ble_store_config_init(void)
{
}

void SimpleBLE::gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op)
    {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGI("BLE_INIT", "registered service %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGI("BLE_INIT", "registering characteristic %s with "
                           "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGI("BLE_INIT", "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}
