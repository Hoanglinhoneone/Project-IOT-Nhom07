package com.smartgarden.controller;

import com.smartgarden.model.HistoryAction;
import com.smartgarden.service.HistoryActionService;
import com.smartgarden.service.MqttPublisherService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;

@RestController
@RequestMapping("/api/devices")
@CrossOrigin(origins = "*")
public class DeviceController {

    @Autowired
    private MqttPublisherService mqttPublisherService;

    @Autowired
    private HistoryActionService historyActionService;

    // Điều khiển thiết bị (bật/tắt)
    @PostMapping("/control")
    public ResponseEntity<String> controlDevice(
            @RequestParam("device") String device,
            @RequestParam("action") String action) {

        if (!device.matches("led|fan|pump")) {
            return ResponseEntity.badRequest().body("Thiết bị không hợp lệ! Chỉ chấp nhận: led, fan, pump.");
        }
        if (!action.matches("ON|OFF")) {
            return ResponseEntity.badRequest().body("Hành động không hợp lệ! Chỉ chấp nhận: ON, OFF.");
        }

        // Gửi lệnh qua MQTT
        switch (device.toLowerCase()) {
            case "led":
                mqttPublisherService.sendLedCommand(action);
                break;
            case "fan":
                mqttPublisherService.sendFanCommand(action);
                break;
            case "pump":
                mqttPublisherService.sendPumpCommand(action);
                break;
        }

        // Lưu lịch sử
        HistoryAction historyAction = new HistoryAction();
        historyAction.setName(device);
        historyAction.setAction("ON".equalsIgnoreCase(action));
        historyAction.setTime(LocalDateTime.now());
        historyActionService.saveHistoryAction(historyAction);

        return ResponseEntity.ok("Đã gửi lệnh " + action + " cho thiết bị " + device);
    }
}

