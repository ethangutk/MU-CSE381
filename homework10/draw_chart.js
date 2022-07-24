
// This script assumes that google charts is loaded before this script
// in the HTML

google.charts.load('current', {'packages':['corechart']});
google.charts.setOnLoadCallback(drawChart);

function drawChart() {
    var data = getChartData();

    var options = {
        title: 'Runtime statistics',
        legend: { position: 'bottom' },
        series: {0: {targetAxisIndex: 0}, 1: {targetAxisIndex: 1}}
    };

    var chart = new google.visualization.LineChart(document.getElementById('chart'));

    chart.draw(data, options);
}
