const MINUTESPERTICK = 5;// log interval dayslog
const SECONDSPERTICK = (MINUTESPERTICK * 60);// log interval 
const LOGDAYS = 1;
const MAXDAYPOINTS = (LOGDAYS * 24 * 60 * 60 / SECONDSPERTICK);
const MAXHOURPOINTS = 3600;

const INFOTABLENAME = "sensorInfoTable";
const REQUESTINTERVAL = 1;  // sec
const DAYLOGINTERVAL = (5 * 60); // min

const NRITEMS = 4;
const lineWidth = 3;

const TABLEROWPWR			= 1;
const TABLEROWVOLTAGE		= 2;

var hourChart;
var dayChart;
var lastTimeStamp = 0;
var dayLogPrescaler = 1;
var firstRequest = true;
var firstTime = true; // for table
var averagedValues = [0, 0, 0, 0];
var nrAverages = 0;
var energyToday =0;

var accumulatedPwr = 0;
var accumulatedDeliveredPwr = 0;
var accumulatedVoltage = 0;

var presc = 3;
var simVal = 0;
var simTs = 0;
var showDeliveredPower = true; 

const Utils = ChartUtils.init();

function initChart() {

    loadCeckboxes();
    window.addEventListener('DOMContentLoaded', function () {
        var ctx = document.getElementById("hourChart");
        hourChart = new Chart(ctx, configHour);
        var ctx = document.getElementById("dayChart");
        dayChart = new Chart(ctx, configDay);
        
        var cb = document.getElementById('cb1');
        if (!cb.checked) {
            data.datasets.splice(1, 1);
            data2.datasets.splice(1, 1);
            showDeliveredPower = false;
        }
        if (SIMULATE)
            //  plotTest();
            simplot();
        timer();
        startTimer();
    });
};


const data = {
    datasets: [
        {
            label: 'Verbruik (W)',
            data: [],
            borderColor: Utils.CHART_COLORS.red,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.red, 0.5),
            borderWidth: lineWidth,
            borderRadius: Number.MAX_VALUE,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        }
        , {
            label: 'Teruggeleverd (W)',
            data: [],
            borderColor: Utils.CHART_COLORS.yellow,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.orange, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        }
    ]
}

const data2 = {
    datasets: [
        {
            label: 'Verbruik (W)',
            data: [],
            borderColor: Utils.CHART_COLORS.red,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.red, 0.5),
            borderWidth: lineWidth,
            borderRadius: Number.MAX_VALUE,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        },
        {
            label: 'Teruggeleverd (W)',
            data: [],
            borderColor: Utils.CHART_COLORS.yellow,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.orange, 0.5),
            borderWidth: lineWidth,
            borderRadius: 5,
            borderSkipped: false,
            cubicInterpolationMode: 'monotone',
        }
    ]
}

const configHour = {
    type: 'line',
    data: data,
    options: {
        responsive: true,
        pointStyle: false,
        animations: false,
        plugins: {
            legend: {
                position: 'top',
            },
            title: {
                display: true,
                text: 'Uurgrafiek'
            }
        }
    }
}

const configDay = {
    type: 'line',
    data: data2,
    options: {
        responsive: true,
        pointStyle: false,
        animations: false,
        plugins: {
            legend: {
                position: 'top'
            },
            title: {
                display: true,
                text: '24 uurgrafiek'
            }
        }
    }
}

function clear() {
    var nrItems = hourChart.data.labels.length;
    var nrDataSets = hourChart.data.datasets.length;
    hourChart.data.labels.splice(0, nrItems);

    for (let idx = 0; idx < nrDataSets; idx++) {
        hourChart.data.datasets[idx].data.splice(0, nrItems);
    }
    hourChart.update();

    nrItems = dayChart.data.labels.length;

    dayChart.data.labels.splice(0, nrItems);

    nrDataSets = dayChart.data.datasets.length;

    for (let idx = 0; idx < nrDataSets; idx++) {
        dayChart.data.datasets[idx].data.splice(0, nrItems);
    }
    dayChart.update();
}

function clearChart() {
    clear();
}

function clearLog() {
    getItem("clearLog");
    clearChart();
}

function startTimer() {
    if (SIMULATE)
        setInterval(function () { timer() }, 500);
    else
        setInterval(function () { timer() }, REQUESTINTERVAL * 1000);
}
function forgetWifi() {
    getItem("forgetWifi");
}


// plots one sample ( array of .= 1 items)
function plot(chart, values, timeStamp) {
    var maxPoints;
    if (chart == dayChart)
        maxPoints = MAXDAYPOINTS;
    else
        maxPoints = MAXHOURPOINTS;

    const data = chart.data;
    if (data.datasets.length > 0) {
        for (let index = 0; index < data.datasets.length; ++index) {
            var value = parseFloat(values[index]);
            data.datasets[index].data.push(value);
        }

        var date = new Date(timeStamp);
        var labelText = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
        data.labels.push(labelText);

        if (data.labels.length > maxPoints) {
            data.labels.splice(0, 1);
            for (let index = 0; index < data.datasets.length; ++index) {
                data.datasets[index].data.splice(0, 1);
            }
        }
    }
}

function plotLog(chart, str) {
    var arr;
    var timeOffset;
    var sampleTime;
    var measTimeLastSample;

    // if (chart == dayChart)
    //     console.log("Log daychart " + str);
    if (str) {
        var arr2 = str.split("\n");

        var nrPoints = arr2.length - 1;
        if (nrPoints > 0) {
            var arr = arr2[nrPoints - 1].split(",");
            measTimeLastSample = arr[0];

            var sec = Date.now();//  / 1000;  // mseconds since 1-1-1970 
            timeOffset = sec - parseFloat(measTimeLastSample) * 1000;

            for (var p = 0; p < nrPoints; p++) {
                arr = arr2[p].split(",");
                if (arr.length >= 3) {
                    sampleTime = parseFloat(arr[0]) * 1000 + timeOffset;
                    arr.splice(0, 1); // remove timestamp (arr[0[]) from array
                    plot(chart, arr, sampleTime);
                }
            }
            chart.update();
        }
    }
}

function timer() {
    var arr;
    var str;
    var deliveredPwr = 0;
    if (SIMULATE) {
        simplot();
        return;
    }
    var cb = document.getElementById('cb1');
    if (cb.checked) {
        if( data.datasets.length == 1 ) {  // add
            data.datasets.push(data.datasets[0] );
            data2.datasets.push(data2.datasets[0] );
            firstRequest = true; //force new chart
            showDeliveredPower = true;
        }
    }
    else {
        if( data.datasets.length == 2 ) { // remove 
            data.datasets.splice(1, 1);
            data2.datasets.splice(1, 1);
             firstRequest = true; //force new chart
             showDeliveredPower = false;
        }
    }

    if (firstRequest) {
        clear();
        arr = getItem("getDayLogMeasValues");
        plotLog(dayChart, arr);
        arr = getItem("getHourLogMeasValues");
        plotLog(hourChart, arr);

    }
    str = getInfo('getInfoValues', 'infoTable'); // make and fill table
    firstRequest = false;

    // read data for actual power and voltage in table
    var tbl = document.getElementById("infoTable");
    var s1 = tbl.rows[TABLEROWPWR].cells[1].innerHTML; // 137 W
    var arr = s1.split(" ");
    var pwr = parseFloat(arr[0]);
    if (tbl.rows.length > 13) { // then 3 phases 
        s1 = tbl.rows[(TABLEROWPWR + 1)].cells[1].innerHTML;
        arr = s1.split(" ");
        pwr += parseFloat(arr[0]);
        s1 = tbl.rows[(TABLEROWPWR + 2)].cells[1].innerHTML;
        arr = s1.split(" ");
        pwr += parseFloat(arr[0]);

        s1 = tbl.rows[(TABLEROWPWR + 3)].cells[1].innerHTML;  // delivered  L1
        arr = s1.split(" ");
        deliveredPwr = parseFloat(arr[0]);

        s1 = tbl.rows[(TABLEROWPWR + 4)].cells[1].innerHTML;  // delivered L2
        arr = s1.split(" ");
        deliveredPwr += parseFloat(arr[0]);

        s1 = tbl.rows[(TABLEROWPWR + 5)].cells[1].innerHTML;  // delivered L3
        arr = s1.split(" ");
        deliveredPwr += parseFloat(arr[0]);
        accumulatedDeliveredPwr += deliveredPwr;

        energyToday += (pwr - deliveredPwr) / 3600; // add energy for this second
    }
    else
        energyToday += pwr / 3600; // add energy for this second

    accumulatedPwr += pwr;

    str = "Verbruik vandaag:" + (energyToday / 1000).toFixed(0) + "kWh,Opgenomen:" + pwr.toFixed(0) + "W";
    if ( showDeliveredPower)
        str = str +",Geleverd:" + deliveredPwr.toFixed(0) + "W";
    
    str = str +"\r";
    makeInfoTable(str, "headTable");
    str = arr[0];
    s1 = tbl.rows[TABLEROWVOLTAGE].cells[1].innerHTML; // 230.5*W
    arr = s1.split("*");
    accumulatedVoltage += parseFloat(arr[0]);

    str = (Date.now()/1000.0) .toFixed(1) + ',' + pwr.toFixed(1) + ',' + deliveredPwr.toFixed(1) + '\n';
   // plotHourArray(str);
    plotLog(hourChart, str);

    presc--;
    //	presc = 0;
    if (presc == 0) {
        presc = MINUTESPERTICK * 60;
        accumulatedPwr /= (MINUTESPERTICK * 60);
        accumulatedDeliveredPwr /= (MINUTESPERTICK * 60);
        str = (Date.now()/1000.0).toFixed(1) + ',' + accumulatedPwr.toFixed(1) + ',' + accumulatedDeliveredPwr.toFixed(1) + '\n';
        accumulatedPwr = 0;
        accumulatedDeliveredPwr = 0;
        plotLog(dayChart, str);
    }
}


function plotTest() {
    var value = [0, 0, 0, 0];
    for (let i = 0; i < 100; ++i) {
        for (let idx = 0; idx < 4; idx++) {
            value[idx] = Math.cos(i / 10) + idx;
        }
        var sec = Date.now();//  / 1000;  // mseconds since 1-1-1970 date
        plot(hourChart, value, sec + i * 10000);
        plot(dayChart, value, sec + i * 10000);
    }
    hourChart.update();
    dayChart.update();
}



function simplot() {
    var value;
    for (let idx = 0; idx < 100; idx++) {
        value = (simTs).toString() + ',';

        for (let idx2 = 0; idx2 < 4; idx2++) {
            value = value + ((1 + Math.cos(simVal) + idx2).toString() + ',');

        }
        value = value + '\n';
        simTs++;
        simVal += 0.02;
    }
    if (presc-- == 0) {
        plotLog(dayChart, value);
        presc = 3;
    }

    plotLog(hourChart, value);

}