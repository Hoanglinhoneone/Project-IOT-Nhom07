package com.smartgarden.controller;

import com.smartgarden.model.HistoryAction;
import com.smartgarden.service.HistoryActionService;
import com.smartgarden.service.MqttPublisherService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@RestController
@RequestMapping("/api/history")
@CrossOrigin(origins = "*")
public class HistoryActionController {

    @Autowired
    private HistoryActionService historyActionService;

    @GetMapping
    public ResponseEntity<List<HistoryAction>> getAllHistory() {
        List<HistoryAction> historyActions = historyActionService.getAllHistoryActions();
        return historyActions.isEmpty()
                ? ResponseEntity.noContent().build()
                : ResponseEntity.ok(historyActions);
    }

    @GetMapping("/auto-mode")
    public ResponseEntity<Boolean> getAutoModeStatus() {
        HistoryAction autoModeAction = historyActionService.getLatestAutoModeAction();

        if (autoModeAction == null) {
            return ResponseEntity.ok(false);
        }

        return ResponseEntity.ok(autoModeAction.getAction());
    }
    @PostMapping("/auto-mode")
    public ResponseEntity<HistoryAction> updateAutoMode(@RequestBody Map<String, Boolean> payload) {
        Boolean action = payload.get("action"); // Lấy giá trị từ đối tượng JSON

        // Tạo bản ghi mới
        HistoryAction autoModeAction = new HistoryAction();
        autoModeAction.setName("auto_mode");
        autoModeAction.setAction(action);
        autoModeAction.setTime(LocalDateTime.now());

        // Lưu vào database
        HistoryAction savedAction = historyActionService.saveHistoryAction(autoModeAction);

        return ResponseEntity.ok(savedAction);
    }


}