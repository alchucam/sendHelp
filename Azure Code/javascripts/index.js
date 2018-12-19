$(document).ready(function () {


  var timeData = [],
    bpmData = [];


  var data = {
    labels: timeData,
    datasets: [
      {
        fill: false,
        label: 'BPM',
        yAxisID: 'BPM',
        borderColor: "rgba(255, 204, 0, 1)",
        pointBoarderColor: "rgba(255, 204, 0, 1)",
        backgroundColor: "rgba(255, 204, 0, 0.4)",
        pointHoverBackgroundColor: "rgba(255, 204, 0, 1)",
        pointHoverBorderColor: "rgba(255, 204, 0, 1)",
        data: bpmData
      }
    ]
  }

  var basicOption = {
    title: {
      display: true,
      text: 'Heart Beat (BPM) Real-time Data',
      fontSize: 36
    },
    scales: {
      yAxes: [{
        id: 'BPM',
        type: 'linear',
        scaleLabel: {
          labelString: 'BPM',
          display: true
        },
        position: 'left',
      }
      /*, {
          id: 'Humidity',
          type: 'linear',
          scaleLabel: {
            labelString: 'Humidity(%)',
            display: true
          },
          position: 'right'
        }
      */]
    }
  }

  //Get the context of the canvas element we want to select
  var ctx = document.getElementById("myChart").getContext("2d");
  var optionsNoAnimation = { animation: false }
  var myLineChart = new Chart(ctx, {
    type: 'line',
    data: data,
    options: basicOption
  });

  var ws = new WebSocket('wss://' + location.host);
  ws.onopen = function () {
    console.log('Successfully connect WebSocket');
  }
  ws.onmessage = function (message) {
    console.log('receive message' + message.data);
    try {
      var obj = JSON.parse(message.data);


      if(!obj.time || !obj.bpm) {
        return;
      }


      timeData.push(obj.time);
      bpmData.push(obj.bpm);
      // only keep no more than 50 points in the line chart
      const maxLen = 50;
      var len = timeData.length;
      if (len > maxLen) {
        timeData.shift();
        bpmData.shift();
      }


        if (obj.bpm > 30){
          document.getElementById("help").innerHTML = "NEEDS HELP!!";
        }
        else{
          document.getElementById("help").innerHTML = "It's ok";
        }
        document.getElementById("x").innerHTML = obj.latitude;
        document.getElementById("y").innerHTML = obj.longtitude; //test before GPS

      myLineChart.update();
    } catch (err) {
      console.error(err);
    }
  }
});
