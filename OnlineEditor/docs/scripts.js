$(function () {
    var $body = $('body');
    var $frames = $('#frames');
    var $hexInput = $('#hex-input');
    var $insertButton = $('#insert-button');
    var $deleteButton = $('#delete-button');
    var $updateButton = $('#update-button');
    var $leds = $('#leds-matrix');
    var $cols, $rows;

    var global_now_color = 0;

    var generator = {
        tableCols: function () {
            var out = ['<table id="cols-list"><tr>'];
            for (var i = 1; i < 9; i++) {
                out.push('<td class="item" data-col="' + i + '">' + i + '</td>');
            }
            out.push('</tr></table>');
            return out.join('');
        },
        tableRows: function () {
            var out = ['<table id="rows-list">'];
            for (var i = 1; i < 9; i++) {
                out.push('<tr><td class="item" data-row="' + i + '">' + i + '</td></tr>');
            }
            out.push('</table>');
            return out.join('');
        },
        tableLeds: function () {
            var out = ['<table id="leds-matrix">'];
            for (var i = 1; i < 9; i++) {
                out.push('<tr>');
                for (var j = 1; j < 9; j++) {
                    out.push('<td class="item colors-255" data-row="' + i + '" data-col="' + j + '" data-color=255></td>');
                }
                out.push('</tr>');
            }
            out.push('</table>');
            return out.join('');
        },
        // 颜色选择器
        colorPicker: function() {
            var out = ['<table id="color-picker">'];
            var count = 0;
            out.push('<tr>');
            for (var i=0;i<256;i++)
            {
                out.push('<td class="picker color-' + count + '" color-number="' + count +'" ></td>');
                count++;
            }
            out.push('</tr>');
            out.push('</table>');
            return out.join('');
        }
    };
    // 转换器
    var converter = {
        // pattern: 64位hex数据; frame: 略缩图
        patternToFrame: function (pattern) {
            var out = ['<table class="frame" data-hex="' + pattern + '">'];
            for (var i = 0; i < 8; i++) {
                var uint64 = pattern.substr(16*i,16);
                out.push('<tr>');
                for (var j = 0; j < 8; j++) {
                    var uint8 = uint64.substr(2*j,2);
                    // color = 0~255
                    var color = parseInt(uint8, 16);
                    out.push('<td class="item color-'+color+'"></td>');
                }
                out.push('</tr>');
            }
            out.push('</table>');
            return out.join('');
        },
        // patter: 64位hex数据; code: arduino uint64_t 数组, 大端模式显示
        patternsToCodeUint64Array: function (patterns) {
            var out = ['uint64_t example[] = {\n\n'];
            for (var i = 0; i < patterns.length; i++) {
                // 128 characters => 64bytes => 8 * uint64_t
                for (var j = 0; j < 8; j++) {
                    // 需要镜像一下，改成大端模式
                    var uint64 = patterns[i].substr(j*16,16);
                    out.push('  0x');
                    for (var k = 7; k >= 0; k--) {
                        out.push(uint64.substr(k*2, 2));
                    }
                    // out.push(uint64);
                    out.push(',\n');
                }
                out.push(' \n');
            }
            out.pop();
            out.pop();
            out.push('\n};\n');
            // out.push('const int example_len = sizeof(example)/8;\n');
            return out.join('');
        },
        // patter: 64位hex数据; code: arduino uint8_t 数组, 大端模式显示
        patternsToCodeUint8Array: function (patterns) {
            var out = ['uint8_t example[] = {\n'];
            for (var i = 0; i < patterns.length; i++) {
                // 128 characters => 64bytes = 64 * uint8_t
                // out.push('  {\n');            
                for (var j = 0; j < 8; j++) {
                    var uint8x8 = patterns[i].substr(j*16,16);
                    out.push('    ');
                    for (var k = 7; k >= 0; k--){
                        var uint8 = uint8x8.substr(k*2,2);
                        out.push('0x');
                        out.push(uint8);
                        out.push(',');
                    }
                    out.push(' \n');
                }
                out.push(' \n');
            }
            out.pop();
            out.pop();
            out.pop();
            out.push('\n};\n');
            return out.join('');
        },
        // pattern去格式
        fixPattern: function (pattern) {
            pattern = pattern.replace(/0x/g, '');
            pattern = pattern.replace(/,/g, '');
            return pattern;
        },
        fixPatterns: function (patterns) {
            for (var i = 0; i < patterns.length; i++) {
                patterns[i] = converter.fixPattern(patterns[i]);
            }
            return patterns;
        }
    };
    // ???
    function makeFrameElement(pattern) {
        pattern = converter.fixPattern(pattern);
        return $(converter.patternToFrame(pattern)).click(onFrameClick);
    }
    // 将led点图的对应值输出到hexinput栏
    function ledsToHex() {
        var out = [];
        for (var i = 1; i < 9; i++) {
            var byte = [];
            for (var j = 1; j < 9; j++) {
                byte.push(getColors(i,j));
            }
            out.push(byte);
        }
        $hexInput.val(out);
    }
    // 获得led点图中某个点的颜色值，16进制字符串
    function getColors(i, j) {
        var temp_color = $leds.find('[data-row=' + i + '][data-col=' + j + '] ').attr('data-color');
        var colors_hex = [];
        colors_hex.push('0x');
        colors_hex.push(('00' + parseInt(temp_color, 10).toString(16)).substr(-2));
        return colors_hex.join('');
    }
    // hexinput栏输出到led点图
    function hexInputToLeds() {
        var val = getInputHexValue();
        // val is string with 128 characters, 2 characters mean 1 color
        for (var i = 1; i < 9; i++) {
            for (var j = 1; j < 9; j++) {
                var temp_color = val.substr(((i-1)*16+(j-1)*2), 2);
                temp_color = parseInt(temp_color, 16).toString();
                $leds.find('[data-row=' + i + '][data-col=' + j + '] ').attr('data-color', temp_color);
                if ($leds.find('[data-row=' + i + '][data-col=' + j + '] ').hasClass('active')) {
                    $leds.find('[data-row=' + i + '][data-col=' + j + '] ').removeClass().addClass('item active color-'+temp_color);                    
                }
                else {
                    $leds.find('[data-row=' + i + '][data-col=' + j + '] ').removeClass().addClass('item color-'+temp_color);                    
                }
            }
        }
    }

    var savedHashState;

    function printArduinoCode(patterns) {
        if (patterns.length) {
            var code;
            if ($('#images-as-byte-arrays').prop("checked")) {
                code = converter.patternsToCodeUint8Array(patterns);
            } else {
                code = converter.patternsToCodeUint64Array(patterns);
            }
            $('#output').val(code);
        }
    }
    // pattern: 64位hex数据; frame: 略缩图
    function framesToPatterns() {
        var out = [];
        $frames.find('.frame').each(function () {
            out.push($(this).attr('data-hex'));
        });
        return out;
    }

    function saveState() {
        var patterns = framesToPatterns();
        printArduinoCode(patterns);
        window.location.hash = savedHashState = patterns.join('|');
    }

    function loadState() {
        savedHashState = window.location.hash.slice(1);
        $frames.empty();
        var frame;
        var patterns = savedHashState.split('|');
        patterns = converter.fixPatterns(patterns);

        for (var i = 0; i < patterns.length; i++) {
            frame = makeFrameElement(patterns[i]);
            $frames.append(frame);
        }
        frame.addClass('selected');
        // $hexInput.val(frame.attr('data-hex'));
        // hexinput不能直接拿data-hex的值，需要加0x和，
        var out=[];
        var data=frame.attr('data-hex');
        if (data === '') data = 'ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff';
        for (var i=0;i<8;i++){
            var uint64 = data.substr(16*i,16);
            for (var j=0;j<8;j++) {
                var uint8 = uint64.substr(2*j,2);
                out.push('0x');
                out.push(uint8);
                out.push(',');
            }
        }
        out.pop();
        $hexInput.val(out.join(''));
        printArduinoCode(patterns);
        hexInputToLeds();
    }

    function getInputHexValue() {
        // console.log(converter.fixPattern($hexInput.val()));
        return converter.fixPattern($hexInput.val());
    }

    // 略缩图点击后，获取略缩图的data-hex，输出到hexinput栏，再输出到led点图
    function onFrameClick() {
        // $hexInput.val($(this).attr('data-hex'));
        var out=[];
        var data=$(this).attr('data-hex');
        for (var i=0;i<8;i++){
            var uint64 = data.substr(16*i,16);
            for (var j=0;j<8;j++) {
                var uint8 = uint64.substr(2*j,2);
                out.push('0x');
                out.push(uint8);
                out.push(',');
            }
        }
        out.pop();
        $hexInput.val(out.join(''));
        processToSave($(this));
        hexInputToLeds();
    }

    function processToSave($focusToFrame) {
        $frames.find('.frame.selected').removeClass('selected');

        if ($focusToFrame.length) {
            $focusToFrame.addClass('selected');
            $deleteButton.removeAttr('disabled');
            $updateButton.removeAttr('disabled');
        } else {
            $deleteButton.attr('disabled', 'disabled');
            $updateButton.attr('disabled', 'disabled');
        }
        saveState();
    }

    $('#cols-container').append($(generator.tableCols()));
    $('#rows-container').append($(generator.tableRows()));
    $('#leds-container').append($(generator.tableLeds()));
    $('#color-container').append($(generator.colorPicker()));

    $cols = $('#cols-list');
    $rows = $('#rows-list');
    $leds = $('#leds-matrix');
    $picker = $('#color-picker');

    // $picker.find('.item').mousedown( function () {
    //     $(this).removeClass().addClass(global_now_color).addClass('item');
    // });

    $leds.find('.item').mousedown(function () {
        $(this).toggleClass('active');
        if ($(this).hasClass('active')) {
            $(this).removeClass().addClass("color-"+global_now_color.toString()).addClass('item active');
            $(this).attr("data-color", global_now_color.toString());
            // $(this).data-color = global_now_color.toString();
        }
        else {
            $(this).removeClass().addClass('color-255 item');
            $(this).attr("data-color", "255");
        }
        ledsToHex();
    });


    $('#shift-up-button').click(function () {
        var val = getInputHexValue();
        var out = [];
        for (var i = 1; i < 8; i++) {
            // 从第二行颜色开始拿7行，放进out
            var uint64 = val.substr(i * 16, 16);
            out.push(uint64);
        }
        out.push('ffffffffffffffff');
        val = out.join('');
        $hexInput.val(dataToPatterns(val));
        hexInputToLeds();
    });

    $('#shift-down-button').click(function () {
        var val = getInputHexValue();
        var out = [];
        out.push('ffffffffffffffff');
        for (var i = 0; i < 7; i++) {
            // 从第一行颜色开始拿7行，放进out
            var uint64 = val.substr(i * 16, 16);
            out.push(uint64);
        }
        val = out.join('');
        $hexInput.val(dataToPatterns(val));
        hexInputToLeds();
    });

    $('#shift-right-button').click(function () {
        var val = getInputHexValue();
        var out = [];
        for (var i = 0; i < 8; i++) {
            // 拿一行颜色
            var uint64 = val.substr(i * 16, 16);
            // 右移一位
            uint64 = ('ff'+ uint64).substr(0,16);
            out.push(uint64);
        }
        val = out.join('');
        $hexInput.val(dataToPatterns(val));
        hexInputToLeds();
    });

    $('#shift-left-button').click(function () {
        var val = getInputHexValue();
        var out = [];
        for (var i = 0; i < 8; i++) {
            // 拿一行颜色
            var uint64 = val.substr(i * 16, 16);
            // 左移一位
            uint64 = (uint64+'ff').substr(-16);
            out.push(uint64);
        }
        val = out.join('');
        $hexInput.val(dataToPatterns(val));
        hexInputToLeds();
    });


    function dataToPatterns(data) {
        var out = [];
        for (var i=0;i<64;i++){
            out.push('0x');
            out.push(data.substr(i*2,2));
            out.push(',');
        }
        out.pop();
        return out.join('');
    }

    $cols.find('.item').mousedown(function () {
        var col = $(this).attr('data-col');
        // $leds.find('.item[data-col=' + col + ']').toggleClass('active',
        //     $leds.find('.item[data-col=' + col + '].active').length != 8);
        $leds.find('.item[data-col=' + col + ']').toggleClass('active');
        if ($leds.find('.item[data-col=' + col + ']').hasClass('active')) {
            $leds.find('.item[data-col=' + col + ']').removeClass().addClass("color-"+global_now_color.toString()).addClass('item active');
            $leds.find('.item[data-col=' + col + ']').attr("data-color", global_now_color.toString());
            // $(this).data-color = global_now_color.toString();
        }
        else {
            $leds.find('.item[data-col=' + col + ']').removeClass().addClass('color-255 item');
            $leds.find('.item[data-col=' + col + ']').attr("data-color", "255");
        }
        ledsToHex();
    });

    $rows.find('.item').mousedown(function () {
        var row = $(this).attr('data-row');
        // $leds.find('.item[data-row=' + row + ']').toggleClass('active',
        //     $leds.find('.item[data-row=' + row + '].active').length != 8);
        $leds.find('.item[data-row=' + row + ']').toggleClass('active');
        if ($leds.find('.item[data-row=' + row + ']').hasClass('active')) {
            $leds.find('.item[data-row=' + row + ']').removeClass().addClass("color-"+global_now_color.toString()).addClass('item active');
            $leds.find('.item[data-row=' + row + ']').attr("data-color", global_now_color.toString());
            // $(this).data-color = global_now_color.toString();
        }
        else {
            $leds.find('.item[data-row=' + row + ']').removeClass().addClass('color-255 item');
            $leds.find('.item[data-row=' + row + ']').attr("data-color", "255");
        }
        ledsToHex();
    });

    $hexInput.keyup(function () {
        hexInputToLeds();
    });

    $deleteButton.click(function () {
        var $selectedFrame = $frames.find('.frame.selected').first();
        var $nextFrame = $selectedFrame.next('.frame').first();

        if (!$nextFrame.length) {
            $nextFrame = $selectedFrame.prev('.frame').first();
        }

        $selectedFrame.remove();

        if ($nextFrame.length) {
            $hexInput.val($nextFrame.attr('data-hex'));
        }

        processToSave($nextFrame);

        hexInputToLeds();
    });

    $insertButton.click(function () {
        var $newFrame = makeFrameElement(getInputHexValue());
        var $selectedFrame = $frames.find('.frame.selected').first();

        if ($selectedFrame.length) {
            $selectedFrame.after($newFrame);
        } else {
            $frames.append($newFrame);
        }

        processToSave($newFrame);
    });

    $updateButton.click(function () {
        var $newFrame = makeFrameElement(getInputHexValue());
        var $selectedFrame = $frames.find('.frame.selected').first();

        if ($selectedFrame.length) {
            $selectedFrame.replaceWith($newFrame);
        } else {
            $frames.append($newFrame);
        }

        processToSave($newFrame);
    });

    $('#images-as-byte-arrays').change(function () {
        var patterns = framesToPatterns();
        printArduinoCode(patterns);
    });


    $('#matrix-toggle').hover(function () {
        $cols.find('.item').addClass('hover');
        $rows.find('.item').addClass('hover');
    }, function () {
        $cols.find('.item').removeClass('hover');
        $rows.find('.item').removeClass('hover');
    });

    $('#matrix-toggle').mousedown(function () {
        $leds.find('.item').toggleClass('active');
        if ($leds.find('.item').hasClass('active')) {
            $leds.find('.item').removeClass().addClass("color-"+global_now_color.toString()).addClass('item active');
            $leds.find('.item').attr("data-color", global_now_color.toString());
            // $(this).data-color = global_now_color.toString();
        }
        else {
            $leds.find('.item').removeClass().addClass('color-255 item');
            $leds.find('.item').attr("data-color", "255");
        }
        ledsToHex();
    });

    $('#circuit-theme').click(function () {
        if ($body.hasClass('circuit-theme')) {
            $body.removeClass('circuit-theme');
            Cookies.set('page-theme', 'plain-theme', {path: ''});
        } else {
            $body.addClass('circuit-theme');
            Cookies.set('page-theme', 'circuit-theme', {path: ''});
        }
    });

    $('.leds-case').click(function () {
        var themeName = $(this).attr('data-leds-theme');
        global_now_color = $(this).attr('data-leds-theme');
        setLedsTheme(themeName);
        Cookies.set('leds-theme', themeName, {path: ''});
    });

    $('.picker').click(function () {
        global_now_color = $(this).attr('color-number');
        $body.find('#color-show').removeClass().addClass("color-"+global_now_color.toString());
        $body.find('#color-word').html('color you pick: 0x'+ ('00' + parseInt(global_now_color, 10).toString(16)).substr(-2));
    });

    function setLedsTheme(themeName) {
        $body.removeClass().addClass(themeName);
    }

    function setPageTheme(themeName) {
        $body.removeClass('plain-theme circuit-theme').addClass(themeName);
    }

    var playInterval;

    $('#play-button').click(function () {
        if (playInterval) {
            $('#play-button-stop').hide();
            $('#play-button-play').show();
            clearInterval(playInterval);
            playInterval = null;
        } else {
            $('#play-button-stop').show();
            $('#play-button-play').hide();

            playInterval = setInterval(function () {
                var $selectedFrame = $frames.find('.frame.selected').first();
                var $nextFrame = $selectedFrame.next('.frame').first();

                if (!$nextFrame.length) {
                    $nextFrame = $frames.find('.frame').first();
                }

                if ($nextFrame.length) {
                    $hexInput.val($nextFrame.attr('data-hex'));
                }

                processToSave($nextFrame);

                hexInputToLeds();
            }, $('#play-delay-input').val());
        }
    });


    $(window).on('hashchange', function () {
        if (window.location.hash.slice(1) != savedHashState) {
            loadState();
        }
    });

    $frames.sortable({
        stop: function (event, ui) {
            saveState();
        }
    });

    loadState();

    var ledsTheme = Cookies.get('leds-theme');

    if (ledsTheme) {
        setLedsTheme(ledsTheme);
    }

    // var pageTheme = Cookies.get('page-theme') || 'circuit-theme';
    var pageTheme = 'plain-theme';

    setPageTheme(pageTheme);
    ledsToHex();

});
