import QtQuick 2.9

CircualProgress {
    id: root
    property int delay: 3000    // ms
    property bool clickable: true

    signal canceled

    Behavior on progress {
        id: progressBehavior
        NumberAnimation { id: numberAnimation }
    }

    Binding {
        // Force checkable to false to get full control over the checked -property
        target: root
        property: "checkable"
        value: false
    }

    onProgressChanged: {
        if (progress === 1.0) {
            __checked = true
            activated()
        }
    }

    on__CheckedChanged: {
        checked = __checked
        if (__checked) {
            if (progress < 1) {
                progressBehavior.enabled = false
                progress = 1;
                progressBehavior.enabled = true
            }
        } else {
            progressBehavior.enabled = false
            progress = 0
            progressBehavior.enabled = true
        }
    }
    /*
    onPressedChanged: {
        if (__checked) {
            if (pressed) {
                __checked = false
                canceled()
            }
        } else {
            startAnimation(pressed)
        }
    }*/

    onClicked: {
        if (!clickable) {
            return
        }

        if (!__checked && (0.0 < progress && progress < 1.0)) {
            cancel()
        }
    }

    function start() {
        reset()
        startAnimation(true)
    }

    function cancel() {
        reset()
        canceled()
    }

    function reset() {
        __checked = false
        progressBehavior.enabled = false
        progress = 0
        if (numberAnimation.running) {
            numberAnimation.stop()
        }

        progressBehavior.enabled = true
    }

    function startAnimation(pressed) {
        var effectiveDelay = pressed ? delay : delay * 0.3
        numberAnimation.duration = Math.max(0, (pressed ? 1 - progress : progress) * effectiveDelay)
        progress = pressed ? 1 : 0
    }
}
