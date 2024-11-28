package com.smartgarden.service;

import com.smartgarden.model.HistoryAction;
import com.smartgarden.repository.HistoryActionRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class HistoryActionService {

    @Autowired
    private HistoryActionRepository historyActionRepository;

    public HistoryAction saveHistoryAction(HistoryAction historyAction) {
        return historyActionRepository.save(historyAction);
    }

    public List<HistoryAction> getAllHistoryActions() {
        return historyActionRepository.findAll();
    }

    public HistoryAction getHistoryActionById(Long id) {
        return historyActionRepository.findById(id).orElse(null);
    }

    public HistoryAction getLatestAutoModeAction() {
        return historyActionRepository.findTopByNameOrderByTimeDesc("auto_mode");
    }
}