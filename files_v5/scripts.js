// LED Matrix Editor — Vanilla JS, keine externen Abhängigkeiten
(function () {
    'use strict';

    // ---------- kleine Cookie-Helfer (ersetzen js-cookie) ----------
    var Cookies = {
        set: function (name, value) {
            document.cookie = encodeURIComponent(name) + '=' + encodeURIComponent(value) + '; path=/; max-age=' + (60 * 60 * 24 * 365);
        },
        get: function (name) {
            var match = document.cookie.match(new RegExp('(?:^|; )' + encodeURIComponent(name) + '=([^;]*)'));
            return match ? decodeURIComponent(match[1]) : undefined;
        }
    };

    document.addEventListener('DOMContentLoaded', function () {

        var $frames = document.getElementById('frames');
        var $hexInput = document.getElementById('hex-input');
        var $insertButton = document.getElementById('insert-button');
        var $deleteButton = document.getElementById('delete-button');
        var $updateButton = document.getElementById('update-button');
        var $output = document.getElementById('output');
        var $byteArraysCheckbox = document.getElementById('images-as-byte-arrays');
        var body = document.body;

        var $leds, $cols, $rows;
        var global_now_color = 0;
        var savedHashState;
        var playInterval;

        // ---------- HTML-Generatoren ----------
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
                        out.push('<td class="item colors-255" data-row="' + i + '" data-col="' + j + '" data-color="255"></td>');
                    }
                    out.push('</tr>');
                }
                out.push('</table>');
                return out.join('');
            },
            colorPicker: function () {
                var out = ['<div id="color-picker">'];
                for (var i = 0; i < 256; i++) {
                    out.push('<span class="picker color-' + i + '" color-number="' + i + '"></span>');
                }
                out.push('</div>');
                return out.join('');
            }
        };

        // ---------- Konvertierung Pattern <-> Code ----------
        var converter = {
            patternToFrame: function (pattern) {
                var out = ['<table class="frame" draggable="true" data-hex="' + pattern + '">'];
                for (var i = 0; i < 8; i++) {
                    var uint64 = pattern.substr(16 * i, 16);
                    out.push('<tr>');
                    for (var j = 0; j < 8; j++) {
                        var uint8 = uint64.substr(2 * j, 2);
                        var color = parseInt(uint8, 16);
                        out.push('<td class="item color-' + color + '"></td>');
                    }
                    out.push('</tr>');
                }
                out.push('</table>');
                return out.join('');
            },
            patternsToCodeUint64Array: function (patterns) {
                var out = ['uint64_t example[] = {\n\n'];
                for (var i = 0; i < patterns.length; i++) {
                    for (var j = 0; j < 8; j++) {
                        var uint64 = patterns[i].substr(j * 16, 16);
                        out.push('  0x');
                        for (var k = 0; k < 8; k++) {
                            out.push(uint64.substr(k * 2, 2));
                        }
                        out.push(',\n');
                    }
                    out.push(' \n');
                }
                out.pop();
                out.pop();
                out.push('\n};\n');
                return out.join('');
            },
            patternsToCodeUint8Array: function (patterns) {
                var out = ['uint8_t example[] = {\n'];
                for (var i = 0; i < patterns.length; i++) {
                    for (var j = 0; j < 8; j++) {
                        var uint8x8 = patterns[i].substr(j * 16, 16);
                        out.push('    ');
                        for (var k = 0; k < 8; k++) {
                            var uint8 = uint8x8.substr(k * 2, 2);
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
            fixPattern: function (pattern) {
                pattern = (pattern || '').replace(/0x/gi, '');
                return pattern.replace(/[^0-9a-fA-F]/g, '').toLowerCase();
            },
            fixPatterns: function (patterns) {
                for (var i = 0; i < patterns.length; i++) {
                    patterns[i] = converter.fixPattern(patterns[i]);
                }
                return patterns;
            }
        };

        function htmlToElement(html) {
            var tmp = document.createElement('template');
            tmp.innerHTML = html.trim();
            return tmp.content.firstChild;
        }

        function makeFrameElement(pattern) {
            pattern = converter.fixPattern(pattern);
            var el = htmlToElement(converter.patternToFrame(pattern));
            el.addEventListener('click', onFrameClick);
            attachDragHandlers(el);
            return el;
        }

        // ---------- LED-Matrix <-> Hex ----------
        function ledsToHex() {
            var out = [];
            for (var i = 1; i < 9; i++) {
                var byte = [];
                for (var j = 1; j < 9; j++) {
                    byte.push(getColors(i, j));
                }
                out.push(byte);
            }
            $hexInput.value = out.join(',');
        }

        function getColors(i, j) {
            var cell = $leds.querySelector('[data-row="' + i + '"][data-col="' + j + '"]');
            var temp_color = parseInt(cell.getAttribute('data-color'), 10);
            if (isNaN(temp_color)) temp_color = 255;
            return '0x' + ('00' + temp_color.toString(16)).slice(-2);
        }

        function hexInputToLeds() {
            var val = getInputHexValue();
            for (var i = 1; i < 9; i++) {
                for (var j = 1; j < 9; j++) {
                    var raw = val.substr(((i - 1) * 16 + (j - 1) * 2), 2);
                    var n = parseInt(raw, 16);
                    if (isNaN(n)) n = 255;
                    var temp_color = n.toString();
                    var cell = $leds.querySelector('[data-row="' + i + '"][data-col="' + j + '"]');
                    var wasActive = cell.classList.contains('active');
                    cell.setAttribute('data-color', temp_color);
                    cell.className = 'item ' + (wasActive ? 'active ' : '') + 'color-' + temp_color;
                }
            }
        }

        function printArduinoCode(patterns) {
            if (patterns.length) {
                var code;
                if ($byteArraysCheckbox.checked) {
                    code = converter.patternsToCodeUint8Array(patterns);
                } else {
                    code = converter.patternsToCodeUint64Array(patterns);
                }
                $output.value = code;
            }
        }

        // ---------- Rohdaten ohne "uint64_t ... = { ... };"-Klammern ----------
        function getRawCode(patterns) {
            if (!patterns.length) return '';
            var byteMode = $byteArraysCheckbox.checked;
            var prefix = byteMode ? 'uint8_t example[] = {\n' : 'uint64_t example[] = {\n\n';
            var suffix = '\n};\n';
            var code = byteMode
                ? converter.patternsToCodeUint8Array(patterns)
                : converter.patternsToCodeUint64Array(patterns);
            return code.slice(prefix.length, code.length - suffix.length);
        }

        function copyToClipboard(text, $button) {
            function showCopied() {
                if (!$button) return;
                var original = $button.textContent;
                $button.textContent = '✓ kopiert';
                $button.classList.add('copied');
                setTimeout(function () {
                    $button.textContent = original;
                    $button.classList.remove('copied');
                }, 1500);
            }

            if (navigator.clipboard && navigator.clipboard.writeText) {
                navigator.clipboard.writeText(text).then(showCopied, function () {
                    fallbackCopy(text);
                    showCopied();
                });
            } else {
                fallbackCopy(text);
                showCopied();
            }
        }

        function fallbackCopy(text) {
            var $tmp = document.createElement('textarea');
            $tmp.value = text;
            $tmp.style.position = 'fixed';
            $tmp.style.opacity = '0';
            document.body.appendChild($tmp);
            $tmp.focus();
            $tmp.select();
            try {
                document.execCommand('copy');
            } catch (e) { /* ignore */ }
            document.body.removeChild($tmp);
        }

        function framesToPatterns() {
            var out = [];
            $frames.querySelectorAll('.frame').forEach(function (f) {
                out.push(f.getAttribute('data-hex'));
            });
            return out;
        }

        function updateFrameCount(count) {
            var $count = document.getElementById('frame-count');
            $count.textContent = count + (count === 1 ? ' Bild' : ' Bilder');
        }

        function saveState() {
            var patterns = framesToPatterns();
            printArduinoCode(patterns);
            updateFrameCount(patterns.length);
            savedHashState = patterns.join('|');
            window.location.hash = savedHashState;
        }

        function loadState() {
            savedHashState = window.location.hash.slice(1);
            $frames.innerHTML = '';
            var frame;
            var patterns = savedHashState.split('|');
            patterns = converter.fixPatterns(patterns);

            for (var i = 0; i < patterns.length; i++) {
                frame = makeFrameElement(patterns[i]);
                $frames.appendChild(frame);
            }
            updateFrameCount(patterns.length && patterns[0] !== '' ? patterns.length : 0);
            if (!frame) return;
            frame.classList.add('selected');

            var out = [];
            var data = frame.getAttribute('data-hex');
            if (data === '') data = 'ff'.repeat(64);
            for (var i2 = 0; i2 < 8; i2++) {
                var uint64 = data.substr(16 * i2, 16);
                for (var j = 0; j < 8; j++) {
                    var uint8 = uint64.substr(2 * j, 2);
                    out.push('0x');
                    out.push(uint8);
                    out.push(',');
                }
            }
            out.pop();
            $hexInput.value = out.join('');
            printArduinoCode(patterns);
            hexInputToLeds();
        }

        function getInputHexValue() {
            return converter.fixPattern($hexInput.value);
        }

        function onFrameClick() {
            var out = [];
            var data = this.getAttribute('data-hex');
            for (var i = 0; i < 8; i++) {
                var uint64 = data.substr(16 * i, 16);
                for (var j = 0; j < 8; j++) {
                    var uint8 = uint64.substr(2 * j, 2);
                    out.push('0x');
                    out.push(uint8);
                    out.push(',');
                }
            }
            out.pop();
            $hexInput.value = out.join('');
            processToSave(this);
            hexInputToLeds();
        }

        function processToSave(focusToFrame) {
            var selected = $frames.querySelector('.frame.selected');
            if (selected) selected.classList.remove('selected');

            if (focusToFrame) {
                focusToFrame.classList.add('selected');
                $deleteButton.removeAttribute('disabled');
                $updateButton.removeAttribute('disabled');
            } else {
                $deleteButton.setAttribute('disabled', 'disabled');
                $updateButton.setAttribute('disabled', 'disabled');
            }
            saveState();
        }

        function dataToPatterns(data) {
            var out = [];
            for (var i = 0; i < 64; i++) {
                out.push('0x');
                out.push(data.substr(i * 2, 2));
                out.push(',');
            }
            out.pop();
            return out.join('');
        }

        // ---------- natives Drag & Drop (ersetzt jQuery UI sortable) ----------
        var draggedEl = null;

        function attachDragHandlers(el) {
            el.addEventListener('dragstart', function () {
                draggedEl = el;
                el.classList.add('dragging');
            });
            el.addEventListener('dragend', function () {
                el.classList.remove('dragging');
                draggedEl = null;
                saveState();
            });
            el.addEventListener('dragover', function (e) {
                e.preventDefault();
                if (!draggedEl || draggedEl === el) return;
                var rect = el.getBoundingClientRect();
                var before = (e.clientX - rect.left) < rect.width / 2;
                $frames.insertBefore(draggedEl, before ? el : el.nextSibling);
            });
        }

        $frames.addEventListener('dragover', function (e) {
            e.preventDefault();
        });

        // ---------- Build DOM: Matrix, Header, Farbwähler ----------
        document.getElementById('cols-container').innerHTML = generator.tableCols();
        document.getElementById('rows-container').innerHTML = generator.tableRows();
        document.getElementById('leds-container').innerHTML = generator.tableLeds();
        document.getElementById('color-container').innerHTML = generator.colorPicker();

        $cols = document.getElementById('cols-list');
        $rows = document.getElementById('rows-list');
        $leds = document.getElementById('leds-matrix');
        var $picker = document.getElementById('color-picker');

        $leds.querySelectorAll('.item').forEach(function (item) {
            item.addEventListener('mousedown', function () {
                this.classList.toggle('active');
                if (this.classList.contains('active')) {
                    this.className = 'item active color-' + global_now_color;
                    this.setAttribute('data-color', String(global_now_color));
                } else {
                    this.className = 'color-255 item';
                    this.setAttribute('data-color', '255');
                }
                ledsToHex();
            });
        });

        // ---------- Shift-Buttons ----------
        document.getElementById('shift-up-button').addEventListener('click', function () {
            var val = getInputHexValue();
            var out = [];
            for (var i = 1; i < 8; i++) {
                out.push(val.substr(i * 16, 16));
            }
            out.push('ffffffffffffffff');
            val = out.join('');
            $hexInput.value = dataToPatterns(val);
            hexInputToLeds();
        });

        document.getElementById('shift-down-button').addEventListener('click', function () {
            var val = getInputHexValue();
            var out = ['ffffffffffffffff'];
            for (var i = 0; i < 7; i++) {
                out.push(val.substr(i * 16, 16));
            }
            val = out.join('');
            $hexInput.value = dataToPatterns(val);
            hexInputToLeds();
        });

        document.getElementById('shift-right-button').addEventListener('click', function () {
            var val = getInputHexValue();
            var out = [];
            for (var i = 0; i < 8; i++) {
                var uint64 = val.substr(i * 16, 16);
                uint64 = ('ff' + uint64).substr(0, 16);
                out.push(uint64);
            }
            val = out.join('');
            $hexInput.value = dataToPatterns(val);
            hexInputToLeds();
        });

        document.getElementById('shift-left-button').addEventListener('click', function () {
            var val = getInputHexValue();
            var out = [];
            for (var i = 0; i < 8; i++) {
                var uint64 = val.substr(i * 16, 16);
                uint64 = (uint64 + 'ff').substr(-16);
                out.push(uint64);
            }
            val = out.join('');
            $hexInput.value = dataToPatterns(val);
            hexInputToLeds();
        });

        // ---------- Spalten-/Zeilen-Header ----------
        $cols.querySelectorAll('.item').forEach(function (item) {
            item.addEventListener('mousedown', function () {
                var col = this.getAttribute('data-col');
                var cells = $leds.querySelectorAll('.item[data-col="' + col + '"]');
                var willActivate = !cells[0].classList.contains('active');
                cells.forEach(function (cell) {
                    if (willActivate) {
                        cell.className = 'item active color-' + global_now_color;
                        cell.setAttribute('data-color', String(global_now_color));
                    } else {
                        cell.className = 'color-255 item';
                        cell.setAttribute('data-color', '255');
                    }
                });
                ledsToHex();
            });
        });

        $rows.querySelectorAll('.item').forEach(function (item) {
            item.addEventListener('mousedown', function () {
                var row = this.getAttribute('data-row');
                var cells = $leds.querySelectorAll('.item[data-row="' + row + '"]');
                var willActivate = !cells[0].classList.contains('active');
                cells.forEach(function (cell) {
                    if (willActivate) {
                        cell.className = 'item active color-' + global_now_color;
                        cell.setAttribute('data-color', String(global_now_color));
                    } else {
                        cell.className = 'color-255 item';
                        cell.setAttribute('data-color', '255');
                    }
                });
                ledsToHex();
            });
        });

        $hexInput.addEventListener('keyup', hexInputToLeds);

        // ---------- Insert / Update / Delete ----------
        $deleteButton.addEventListener('click', function () {
            var $selectedFrame = $frames.querySelector('.frame.selected');
            if (!$selectedFrame) return;
            var $nextFrame = $selectedFrame.nextElementSibling || $selectedFrame.previousElementSibling;

            $selectedFrame.remove();

            if ($nextFrame) {
                $hexInput.value = $nextFrame.getAttribute('data-hex');
            }
            processToSave($nextFrame);
            hexInputToLeds();
        });

        $insertButton.addEventListener('click', function () {
            var $newFrame = makeFrameElement(getInputHexValue());
            var $selectedFrame = $frames.querySelector('.frame.selected');

            if ($selectedFrame) {
                $selectedFrame.insertAdjacentElement('afterend', $newFrame);
            } else {
                $frames.appendChild($newFrame);
            }
            processToSave($newFrame);
        });

        $updateButton.addEventListener('click', function () {
            var $newFrame = makeFrameElement(getInputHexValue());
            var $selectedFrame = $frames.querySelector('.frame.selected');

            if ($selectedFrame) {
                $selectedFrame.replaceWith($newFrame);
            } else {
                $frames.appendChild($newFrame);
            }
            processToSave($newFrame);
        });

        $byteArraysCheckbox.addEventListener('change', function () {
            printArduinoCode(framesToPatterns());
        });

        document.getElementById('copy-image-button').addEventListener('click', function () {
            var pattern = getInputHexValue();
            var text = getRawCode([pattern]);
            copyToClipboard(text, this);
        });

        document.getElementById('copy-animation-button').addEventListener('click', function () {
            var text = getRawCode(framesToPatterns());
            copyToClipboard(text, this);
        });

        document.getElementById('save-code-button').addEventListener('click', function () {
            var patterns = framesToPatterns();
            if (!patterns.length) return;
            var byteMode = $byteArraysCheckbox.checked;
            var code = byteMode
                ? converter.patternsToCodeUint8Array(patterns)
                : converter.patternsToCodeUint64Array(patterns);
            var blob = new Blob([code], { type: 'text/plain' });
            var url = URL.createObjectURL(blob);
            var $a = document.createElement('a');
            $a.href = url;
            $a.download = 'led-matrix-code.h';
            document.body.appendChild($a);
            $a.click();
            document.body.removeChild($a);
            setTimeout(function () { URL.revokeObjectURL(url); }, 1000);
        });

        // ---------- Matrix-Toggle (alles an/aus) ----------
        var $matrixToggle = document.getElementById('matrix-toggle');
        $matrixToggle.addEventListener('mouseenter', function () {
            $cols.querySelectorAll('.item').forEach(function (i) { i.classList.add('hover'); });
            $rows.querySelectorAll('.item').forEach(function (i) { i.classList.add('hover'); });
        });
        $matrixToggle.addEventListener('mouseleave', function () {
            $cols.querySelectorAll('.item').forEach(function (i) { i.classList.remove('hover'); });
            $rows.querySelectorAll('.item').forEach(function (i) { i.classList.remove('hover'); });
        });
        $matrixToggle.addEventListener('mousedown', function () {
            var items = $leds.querySelectorAll('.item');
            var willActivate = !items[0].classList.contains('active');
            items.forEach(function (item) {
                if (willActivate) {
                    item.className = 'item active color-' + global_now_color;
                    item.setAttribute('data-color', String(global_now_color));
                } else {
                    item.className = 'color-255 item';
                    item.setAttribute('data-color', '255');
                }
            });
            ledsToHex();
        });

        // ---------- Themes ----------
        document.getElementById('circuit-theme').addEventListener('click', function () {
            if (body.classList.contains('circuit-theme')) {
                body.classList.remove('circuit-theme');
                Cookies.set('page-theme', 'plain-theme');
            } else {
                body.classList.add('circuit-theme');
                Cookies.set('page-theme', 'circuit-theme');
            }
        });

        document.querySelectorAll('.leds-case').forEach(function (el) {
            el.addEventListener('click', function () {
                var themeName = this.getAttribute('data-leds-theme');
                setLedsTheme(themeName);
                Cookies.set('leds-theme', themeName);
            });
        });

        function setDrawColor(colorNumber) {
            global_now_color = colorNumber;
            document.getElementById('color-show').className = 'color-' + global_now_color;
            document.getElementById('color-word').innerHTML = '0x' + ('00' + global_now_color.toString(16)).slice(-2);
        }

        $picker.querySelectorAll('.picker').forEach(function (el) {
            el.addEventListener('click', function () {
                setDrawColor(parseInt(this.getAttribute('color-number'), 10));
            });
        });

        // ---------- Favoriten (5 dauerhaft gespeicherte Farben) ----------
        var FAVORITES_KEY = 'tueftelpark-led-favorites';
        var $favorites = document.getElementById('favorites');

        function loadFavorites() {
            var favs = [null, null, null, null, null];
            try {
                var raw = localStorage.getItem(FAVORITES_KEY);
                if (raw) {
                    var parsed = JSON.parse(raw);
                    if (Array.isArray(parsed)) {
                        for (var i = 0; i < 5; i++) {
                            favs[i] = (typeof parsed[i] === 'number') ? parsed[i] : null;
                        }
                    }
                }
            } catch (e) { /* ignore */ }
            return favs;
        }

        var favorites = loadFavorites();

        function saveFavorites() {
            try {
                localStorage.setItem(FAVORITES_KEY, JSON.stringify(favorites));
            } catch (e) { /* ignore */ }
        }

        function renderFavorites() {
            $favorites.innerHTML = '';
            favorites.forEach(function (color, idx) {
                var $slot = document.createElement('span');
                var filled = (color !== null);
                $slot.className = 'favorite-slot' + (filled ? ' filled color-' + color : '');
                $slot.title = filled
                    ? ('Farbe 0x' + ('00' + color.toString(16)).slice(-2) + ' auswählen (Rechtsklick: leeren)')
                    : 'Aktuelle Farbe hier speichern';

                $slot.addEventListener('click', function () {
                    if (favorites[idx] === null) {
                        favorites[idx] = global_now_color;
                        saveFavorites();
                        renderFavorites();
                    } else {
                        setDrawColor(favorites[idx]);
                    }
                });

                $slot.addEventListener('contextmenu', function (e) {
                    e.preventDefault();
                    favorites[idx] = null;
                    saveFavorites();
                    renderFavorites();
                });

                $favorites.appendChild($slot);
            });
        }

        renderFavorites();

        function setLedsTheme(themeName) {
            body.className = themeName;
        }

        function setPageTheme(themeName) {
            body.classList.remove('plain-theme', 'circuit-theme');
            body.classList.add(themeName);
        }

        // ---------- Play ----------
        document.getElementById('play-button').addEventListener('click', function () {
            var $playIcon = document.getElementById('play-button-play');
            var $stopIcon = document.getElementById('play-button-stop');

            if (playInterval) {
                $stopIcon.style.display = 'none';
                $playIcon.style.display = '';
                clearInterval(playInterval);
                playInterval = null;
            } else {
                $stopIcon.style.display = '';
                $playIcon.style.display = 'none';

                playInterval = setInterval(function () {
                    var $selectedFrame = $frames.querySelector('.frame.selected');
                    var $nextFrame = ($selectedFrame && $selectedFrame.nextElementSibling) || $frames.querySelector('.frame');

                    if ($nextFrame) {
                        $hexInput.value = $nextFrame.getAttribute('data-hex');
                    }
                    processToSave($nextFrame);
                    hexInputToLeds();
                }, parseInt(document.getElementById('play-delay-input').value, 10) || 500);
            }
        });

        window.addEventListener('hashchange', function () {
            if (window.location.hash.slice(1) !== savedHashState) {
                loadState();
            }
        });

        // ---------- Bibliotheks-Links (Sets) ----------
        document.querySelectorAll('#sets a').forEach(function (a) {
            a.addEventListener('click', function () {
                // Hash wird nativ vom Browser gesetzt -> hashchange-Handler übernimmt das Laden
            });
        });

        // ---------- Init ----------
        loadState();

        var ledsTheme = Cookies.get('leds-theme');
        if (ledsTheme) setLedsTheme(ledsTheme);

        setPageTheme('plain-theme');
        ledsToHex();
    });
})();
