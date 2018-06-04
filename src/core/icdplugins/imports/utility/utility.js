//
var warningDict = [
            qsTr("无效"),
            qsTr("故障"),
            qsTr("告警"),
            qsTr("异常"),
            qsTr("过热"),
            qsTr("过冷"),
            qsTr("欠压"),
            qsTr("过压"),
            qsTr("过大"),
            qsTr("失效"),
            qsTr("不平衡"),
            qsTr("未点火"),
            qsTr("未投网"),
            // for landing-brake
            qsTr("模型监控故障"),
            qsTr("开环故障"),
            qsTr("停转故障")
        ];

function isWarning(name) {
    return (warningDict.indexOf(name) >= 0);
}

function searchItem(table, object, callback) {
    var execCallback = function(success) {
        if (callback) {
            callback(success)
        }
    }
    if (!JDataChannelMgr) {
        execCallback(false);
    }
    var result = JDataChannelMgr.search(table, object, function(success, message) {
        execCallback(success)
        if (success) {
            //
        } else {
            notifyDialog.show(qsTr("协议表") +
                              "[<font color=red>" + table.name + "</font>]" +
                              qsTr("中不存在以下协议字段"),
                              message)
        }
    })
    if (!result) {
        console.log("search failure!")
        return false
    }

    return true
}

function pmaTable() {
    var notify = function() {
        notifyDialog.show(qsTr("表协议未找到！"), qsTr("PMA协议"))
    }
    var protocol = JFilterChannel.protocol
    if (!protocol || !protocol.valid) {
        notify()
        return null
    }
    var table = protocol.table
    if (!table) {
        notify()
        return null
    }
    return table
}

function findChildTable(tableObject) {
    var notify = function() {
        notifyDialog.show(qsTr("表协议未找到！"), tableObject.tableName)
    }
    var protocol = JFilterChannel.protocol
    if (!protocol || !protocol.valid) {
        notify()
        return null
    }
    var table = protocol.table
    if (!table) {
        notify()
        return null
    }
    var childTable = table.tableByDomain(tableObject.domain, tableObject.domainType)
    if (!childTable) {
        notify()
        return null
    }
    return childTable
}

function cancelSearch(target) {
    if (JDataChannelMgr) {
        JDataChannelMgr.cancelSearch(target);
    }
}

function loadProtocol(protocol, callback) {
    var notify = function(message) {
        notifyDialog.show(message, protocol.tableName)
    }
    var execCallback = function(success) {
        if (callback) {
            callback(success)
        }
    }
    if (!protocol.load(function(success, error) {
        execCallback(success)
        if (success) {
            //
        } else {
            notify(qsTr("协议加载失败！"))
        }
    })) {
        execCallback(false)
        notify(qsTr("协议基本信息错误，加载失败！"))
    }
}

function textOfValid(isValid) {
    return isValid ? qsTr("有效") : qsTr("无效")
}

function textOfFault(isFault) {
    return isFault ? qsTr("故障") : qsTr("正常")
}

function textOfSignal(isValid) {
    return isValid ? qsTr("信号有效") : qsTr("信号无效")
}

function textOfElectrify(isValid, validFlag) {
    return (isValid === validFlag)
            ? qsTr("已<font size=4 color=yellow>上</font>电")
            : qsTr("已<font size=4 color=yellow>下</font>电")
}

function textOfGround(valid) {
    return valid ? qsTr("接地") : qsTr("空中")
}

function textOfInPlace(valid) {
    return valid ? qsTr("到位") : qsTr("未到位")
}

function textOfEnabled(enabled) {
    return enabled ? qsTr("使能") : qsTr("未使能")
}

function textOfGiven(valid, pair) {
    return valid ? pair[0] : pair[1]
}

function stringOfItem(item) {
    var unit = item.unit
    if (unit && unit.length > 0) {
        return item.valueString + " " + unit
    } else {
        return item.valueString
    }
}

function titleOfItem(item) {
    if (item.itemType === IcdCore.ItemNumeric) {
        return item.name + qsTr("（") + item.unit + qsTr("）")
    } else {
        return item.name
    }
}

function bindingGauge(target, value) {
    target.title = value.name
    target.suffix = value.unit
    target.autoScaled(value.limit.minimum, value.limit.maximum, 8)
    target.value = Qt.binding(function() {
        return value.data
    })
}

function bindingGauge2(target, value, intervals) {
    target.title = value.name
    target.suffix = value.unit
    target.autoScaled(value.limit.minimum, value.limit.maximum, intervals)
    target.value = Qt.binding(function() {
        return value.data
    })
}

function bindingValueChart(target, value) {
    target.title = value.name
    target.suffix = value.unit
    target.value = Qt.binding(function() {
        var data = value.data
        target.shiftValue(data)
        return data
    })
}

function bindingLabelValue(target, value) {
    target.title = Qt.binding(function() {
        return value.name
    })
    target.suffix = value.unit
    target.value = Qt.binding(function() {
        return value.data
    })
}

function bindingLabelValueNoTitle(target, value) {
    target.suffix = value.unit
    target.value = Qt.binding(function() {
        return value.data
    })
}

function bindingLabelText(target, value, highlight) {
    target.title = Qt.binding(function() {
        return value.name
    })
    target.value = Qt.binding(function() {
        var text = stringOfItem(value)
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    if (highlight === true) {
        target.highlighting = Qt.binding(function(){
            return isWarning(target.value);
        });
    }
}

function bindingFieldValue(target, value) {
    target.text = Qt.binding(function() {
        return stringOfItem(value)
    })
}

function bindingFieldValid(target, value, bit) {
    target.text = Qt.binding(function() {
        var data = value.data
        var text = textOfValid(value.testBit(bit))
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
}

function bindingLabelValid(target, value, bit) {
    target.value = Qt.binding(function() {
        var data = value.data
        var text = textOfValid(value.testBit(bit))
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    target.highlighting = Qt.binding(function() {
        var data = value.data
        return !value.testBit(bit)
    })
}

function bindingLabelElectrify(target, value, bit, validFlag) {
    target.value = Qt.binding(function() {
        var data = value.data
        var text = textOfElectrify(value.testBit(bit), validFlag)
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
}

function bindingLabelEnabled(target, value, bit, enabledFlag) {
    target.value = Qt.binding(function() {
        var data = value.data
        var text = textOfEnabled(value.testBit(bit), enabledFlag)
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
}

function bindingLabelGround(target, value, bit) {
    target.title = Qt.binding(function() {
        var data = value.data
        var text = value.nameAt(bit)
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    target.value = Qt.binding(function() {
        var data = value.data
        var text = textOfGround(value.testBit(bit))
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    target.highlighting = Qt.binding(function() {
        var data = value.data
        return value.testBit(bit)
    })
}

function bindingLabelInPlace(target, value, bit) {
    target.value = Qt.binding(function() {
        var data = value.data
        var text = textOfInPlace(value.testBit(bit))
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    target.highlighting = Qt.binding(function() {
        var data = value.data
        return value.testBit(bit)
    })
}

function bindingLabelFault(target, value, bit) {
    target.value = Qt.binding(function() {
        var data = value.data
        var text = textOfFault(value.testBit(bit))
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    target.highlighting = Qt.binding(function() {
        var data = value.data
        return value.testBit(bit)
    })
}

function bindingLabelGiven(target, value, bit, pair, highlighting) {
    target.value = Qt.binding(function() {
        var data = value.data
        var text = textOfGiven(value.testBit(bit), pair)
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    if (highlighting) {
        target.highlighting = Qt.binding(function() {
            var data = value.data
            return value.testBit(bit)
        })
    } else {
        target.highlighting = Qt.binding(function(){
            return isWarning(target.value)
        });
    }
}

function bindingLabelVolume(target, value) {
    target.title = Qt.binding(function() {
        return value.name
    })
    target.suffix = value.unit
    target.value = Qt.binding(function() {
        return value.data
    })
}

function bindingLabelBitValue(target, value, highlight) {
    if (!value) {
        target.title = qsTr("--")
        target.value = qsTr("--")
        return
    }
    target.title = Qt.binding(function() {
        return value.name
    })
    target.value = Qt.binding(function() {
        var text = value.specAt(value.data)
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    if (highlight !== undefined) {
        target.highlighting = Qt.binding(function(){
            return (value.data === highlight);
        });
    } else {
        target.highlighting = Qt.binding(function(){
            return isWarning(value.specAt(value.data));
        })
    }
}

function bindingLabelBitValueNoTitle(target, value, highlight) {
    target.value = Qt.binding(function() {
        var text = value.specAt(value.data)
        if (text.length === 0) {
            text = qsTr("--")
        }
        return text
    })
    if (highlight === true) {
        target.highlighting = Qt.binding(function(){
            return isWarning(target.value);
        });
    }
}

function bindingLabelBitValueValue(target, value) {
    target.title = Qt.binding(function() {
        return value.name
    })
    target.value = Qt.binding(function() {
        return value.data
    })
}

function bindingBoolValue(value, bit, revert) {
    return Qt.binding(function() {
        var data = value.data
        var flag = value.testBit(bit)
        if (revert) {
            flag = !flag
        }
        return flag
    })
}


//====================================================================

function copyJDoubleSpinBox(source, target) {
    target.from = source.from
    target.to = source.to
    target.value = source.value
}

function saveJDoubleSpinBox(source, target) {
    target.from = source.from
    target.to = source.to
    target.value = source.value
}

function restoreJDoubleSpinBox(source, target) {
    target.from = source.from
    target.to = source.to
    target.value = source.value
}

function saveJDoubleSpinBoxNoRange(source, target) {
    target.value = source.value
}

function restoreJDoubleSpinBoxNoRange(source, target) {
    target.value = source.value
}

function saveJTextLabel(source, target) {
    target.value = source.value
}

function restoreJTextLabel(source, target) {
    target.value = source.value
}

function saveJDialLabel(source, target) {
    target.from = source.from
    target.to = source.to
    target.value = source.value
}

function restoreJDialLabel(source, target) {
    target.from = source.from
    target.to = source.to
    target.value = source.value
}

function saveSwitch(source, target) {
    target.checked = source.checked
}

function restoreSwitch(source, target) {
    target.checked = source.checked
}

function saveDialValue(source, target) {
    target.value = source.value
}

function restoreDialValue(source, target) {
    target.value = source.value
}

//====================================================================

function rebootNotify(callback, condition) {
    notifyDialog.await(qsTr("飞管计算机正在软重启，请稍候……"), "",
                       true, callback,
                       function() {
                           return FlyMgrSysModel.currentWorkMode !== JCmdChannel.WorkModeInvalid
                       }, 30000, 2000)
}

function blockingNotify(message, callback) {
    notifyDialog.show(message, "", null, false, "", function() {
        IcdCore.asyncCall(callback, null)
    })
}

//====================================================================

function calcBitCount(bitItem) {
    var count = 0;
    for (var i = 0; i < bitItem.bitCount; ++i) {
        var name = bitItem.nameAt(bitItem.bitStart + i);
        if (name.length > 0) {
            ++count;
        }
    }

    return count;
}

function offsetOfValidBit(value, index) {
    var count = 0;
    for (var i = 0; i < value.bitCount; ++i) {
        var offset = value.bitStart + i;
        var name = value.nameAt(offset);
        if (name.length > 0) {
            ++count;
            if (count - 1 === index) {
                return offset;
            }
        }
    }
    return -1;
}

function bindingLabel(label, value, index, highlight) {
    var offset = offsetOfValidBit(value, index);
    if (offset < 0) {
        return;
    }
    label.title = value.nameAt(offset);
    label.value = Qt.binding(function(){
        var data = value.data;
        var desc = value.descAt(offset);
        if (desc.length > 0) {
            return desc;
        } else {
            return '--';
        }
    });
    if (highlight !== undefined) {
        label.highlighting = Qt.binding(function(){
            var data = value.data;
            return (value.testBit(offset) === highlight);
        })
    } else {
        label.highlighting = Qt.binding(function(){
            var data = value.data;
            return isWarning(value.descAt(offset));
        })
    }
}

function bindingLabelNoFilter(label, value, index, highlight) {
    label.title = value.nameAt(index);
    label.value = Qt.binding(function(){
        var data = value.data;
        var desc = value.descAt(index);
        if (desc.length > 0) {
            return desc;
        } else {
            return '--';
        }
    });
    if (highlight !== undefined) {
        label.highlighting = Qt.binding(function(){
            var data = value.data;
            return (value.testBit(index) === highlight);
        })
    } else {
        label.highlighting = Qt.binding(function(){
            var data = value.data;
            return isWarning(value.descAt(index));
        })
    }
}
