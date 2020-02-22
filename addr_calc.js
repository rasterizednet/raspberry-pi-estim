function voltageDivider(vs, r1, r2) {
    return ( vs * r2 ) / ( r1 + r2 );
}

function addCharacters(count, c) {
    var s = "";
    c = c || " ";
    while(count--) {
        s += c;
    }
    return s;
}


var spacerLengths = [8, 10, 8, 12, 14, 10, 10];
function printResult(i, total, voltage, value, min, max, diff) {
    var bits = (value).toString(2);
    bits = addCharacters(10 - bits.length, "0") + bits
    var spacers = [];
    total = Math.round( total );
    voltage = Math.round( voltage * 1000 ) / 1000;
    voltage = voltage.toString();
    voltage = voltage + addCharacters(5 - voltage.length, "0");
    spacers[0] = addCharacters(spacerLengths[0] - (i).toString().length);
    spacers[1] = addCharacters(spacerLengths[1] - (total).toString().length);
    spacers[2] = addCharacters(spacerLengths[2] - (voltage).toString().length);
    spacers[3] = addCharacters(spacerLengths[3] - (value).toString().length);
    spacers[4] = addCharacters(spacerLengths[4] - (bits).toString().length);
    spacers[5] = addCharacters(spacerLengths[5] - (min.toString(16)).toString().length);
    spacers[6] = addCharacters(spacerLengths[6] - (max.toString(16)).toString().length);
    console.log(
        `${i}${spacers[0]}${total}${spacers[1]}${voltage}` +
        `${spacers[2]}${value}${spacers[3]}${bits}` +
        `${spacers[4]}${min.toString(16)}${spacers[5]}${max.toString(16)}` +
        `${spacers[6]}${diff.toString(16)}`
    );
}

function generateCList(i, min, max) {
    min = min.toString(16);
    min = addCharacters(4 - min.length, "0") + min;
    max = max.toString(16);
    max = addCharacters(4 - max.length, "0") + max;
    i = i.toString(16);
    i = addCharacters(2 - i.length, "0") + i;
    console.log(`{ 0x${min}, 0x${max} }, // 0x${i}`);
}

function csvResult(i, total, voltage, value) {
    var bits = (value).toString(2);
    bits = addCharacters(10 - bits.length, "0") + bits
    total = Math.round( total );
    voltage = Math.round( voltage * 1000 ) / 1000;
    voltage = voltage.toString();
    voltage = voltage + addCharacters(5 - voltage.length, "0");
    console.log(`${i},${total},${voltage},${value},${bits}`);
}

var vBase = 5;
var bitSpacing = 1023;
var vResolution = vBase/bitSpacing;
var rBase = 1000;
var rOffset = 0;
var rList = [1000, 2000, 4000, 8000, 16000];
var bitResolution = Math.pow(2, rList.length);

var results = [];
for (var i=0;i < bitResolution; i++) {
    var sum = 0;
    for (var a=0;a < rList.length; a++) {
        if (i & (1 << a)) {
            sum += 1/rList[a];
        }
    }
    var total = sum === 0 ? 0 : 1/sum;
    total += rOffset;
    var voltage = sum ? voltageDivider(3.3, total, rBase) : 0;
    var value = Math.floor(voltage/vResolution);
    results.push({i, total, voltage, value});
}

results.sort(function(a, b) {
    return a.value - b.value;
});

for(var i=0; i < results.length; i++) {
    var min = 0;
    var max = 0;
    if(i === 0) {
        results[i].min = 0;
        results[i].diff = 0;
    } else {
        results[i].min = results[i].value - Math.floor((results[i].value - results[i-1].value) / 2);
        if(results[i-1].max === results[i].min) {
            results[i].min += 1;
        }
        results[i].diff = results[i].min - results[i-1].min;
    }
    if(i === results.length - 1) {
        results[i].max = bitSpacing;
    } else {
        results[i].max = results[i].value + Math.floor((results[i+1].value - results[i].value) / 2);
    }
}

results.sort(function(a, b) {
    return a.value - b.value;
});

console.log(`Index   R Value   Volts   Decimal     Binary        Min hex   Max hex   Diff hex`);
results.forEach(function(obj) {
    printResult(obj.i, obj.total, obj.voltage, obj.value, obj.min, obj.max, obj.diff);
});

results.sort(function(a, b) {
  return a.i - b.i;
});
console.log("");
console.log("C Array List");
console.log("{ <Min hex>, <Max hex> } // <Index hex>");
results.forEach(function(obj) {
  generateCList(obj.i, obj.min, obj.max);
});