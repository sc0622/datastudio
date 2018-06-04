
var mantissi = [ 1, 2, 5 ]

function convInt(a) {
    if (a > 0) {
        return Math.floor(a)
    } else {
        return Math.ceil(a)
    }
}

function isNull(a, b) {
    if (!b) {
        return Math.abs(a) <= 2.2250738585072014e-308
    } else {
        return Math.abs(a - b) <= Math.min(Math.abs(a), Math.abs(b) * 2.2204460492503131e-016)
    }
}

function floorExt(x) {
    if (x === 0.0) {
        return [0, 0.0]
    }
    var sign = x > 0 ? 1.0 : -1.0
    var lx = Math.log(Math.abs(x))
    var exponent = Math.floor(lx)
    var fr = Math.pow(10.0, lx - exponent)
    if (fr >= 10.0) {
        fr = 1.0
        ++exponent
    } else {
        for (var i = mantissi.length - 1; i >= 0; --i) {
            var item = mantissi[i]
            if (fr >= item) {
                fr = item
                break
            }
        }
    }

    return [exponent, sign * fr]
}

function floor125(x) {
    return floorExt(x)
}

function anchorValue(start, m, n) {
    var stepValue = m * Math.pow(10.0, n)
    return stepValue * Math.ceil(start / stepValue)
}

function segments(start, stop, anchor, m, n) {
    var value = m * Math.pow(10.0, n)
    var delta = (stop - anchor) / value
    var r_intervals = Math.floor(delta) // right side intervals
    delta = (anchor - start) / value
    var l_intervals = Math.floor(delta) // left side intervals
    return [l_intervals + r_intervals, l_intervals, r_intervals]
}

function execute(start, stop, intervals) {
    var a = start, b = stop
    if (intervals < 1) {
        intervals = 1
    }

    var delta = Math.abs(stop - start)
    if (isNull(delta)) {
        return [a, b, intervals]
    }
    var ret = floorExt(delta)
    var n = ret[0], c = ret[1]
    var anchor = anchorValue(start, c, n)
    ret = segments(start, stop, anchor, c, n)
    var ival = ret[0], l_ival = ret[1], r_ival = ret[2]
    if (ival >= intervals) {
        a = anchor - l_ival * c * Math.pow(10.0, n)
        b = anchor + r_ival * c * Math.pow(10.0, n)
        intervals = ival
        return [a, b, intervals]
    }

    var prev_ival = 0, prev_l_ival = 0, prev_r_ival = 0
    var prev_anchor = 0.0, prev_c = 0.0
    var prev_n = 0
    while (true) {
        prev_c = c
        prev_n = n
        prev_anchor = anchor
        prev_ival = ival
        prev_l_ival = l_ival
        prev_r_ival = r_ival
        var ic = convInt(c)
        if (ic === 1) {
            c = mantissi[mantissi.length - 1]
            --n
        } else {
            for (var i = mantissi.length - 1; i > 0; --i) {
                if (ic === mantissi[i]) {
                    c = mantissi[i - 1]
                    break
                }
            }
        }
        anchor = anchorValue(start, c, n)
        ret = segments(start, stop, anchor, c, n)
        ival = ret[0]
        l_ival = ret[1]
        r_ival = ret[2]
        var prev_diff = intervals - prev_ival
        var actual_diff = ival - intervals
        if (prev_diff >= 0 && actual_diff >= 0) {
            if (prev_diff < actual_diff) {
                n = prev_n
                c = prev_c
                anchor = prev_anchor
                ival = prev_ival
                l_ival = prev_l_ival
                r_ival = prev_r_ival
            }
            a = anchor - l_ival * c * Math.pow(10.0, n)
            b = anchor + r_ival * c * Math.pow(10.0, n)
            intervals = ival
            break
        }
    }

    return [a, b, intervals]
}

function autoScaled(start, stop, intervals) {
    var ret = execute(start, stop, intervals)
    var new_start = ret[0], new_stop = ret[1], number = ret[2]
    if (number === 0) {
        return number
    }
    var step = (new_stop - new_start) / number
    if (new_start > start) {
        new_start -= step
        ++number
    }
    if (new_stop < stop) {
        new_stop += step
        ++number
    }
    return [new_start, new_stop, number]
}
