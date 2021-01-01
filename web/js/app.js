var database;
var isSystemOpened;

$(document).ready(function () {
  // you have to use your own firebase.js file for configuration
  // this repository does not follow the firebase.js file.
  firebase.initializeApp(firebaseConfig); // from firebase.js 
  database = firebase.database(); 

  checkSystem(); 
  checkNumberApprovedBreach();
  checkNumberBreach();
  systemOpenClose();
  checkBreachList();
});

function checkSystem() {
  // get system open-close information
  // if system is open you can see green circle
  firebase.database().ref('isSystemOpened').on('value', function(snapshot){
    if(snapshot.val() == 0){
      $('#isSystemOpened').removeClass("green");
      $('#isSystemOpened').addClass("red");
      $('#systemOpenClose').html('Sistemi Aç');
      isSystemOpened = 0;
    } else {
      $('#isSystemOpened').removeClass("red");
      $('#isSystemOpened').addClass("green");
      $('#systemOpenClose').html('Sistemi Kapat');
      isSystemOpened = 1;
    }
  });
}

function checkNumberApprovedBreach(){
  // check the number of confirmed security breaches from firebase
  firebase.database().ref('approvedBreach').on('value', function(snapshot){
    $('#approvedBreach').text(snapshot.val());
    $('#unapprovedBreach').text(parseInt($('#breachNo').text()) - snapshot.val());
  });
}

function checkNumberBreach(){
  // check the number of security breaches from firebase
  firebase.database().ref('breachNo').on('value', function(snapshot){
    $('#breachNo').text(snapshot.val());
    $('#unapprovedBreach').text(snapshot.val() - parseInt($('#approvedBreach').text()));
  });
}

function systemOpenClose(){
  // from here, you can update system state in firebase realtime database
  $('#systemOpenClose').on('click',function(){
    var updates = {};
    updates['/isSystemOpened'] = isSystemOpened == 0 ? 1 : 0;
    firebase.database().ref().update(updates);
  })
}

function checkBreachList(){
  // it pulls all security breaches from firebase and fills them in the table.
  firebase.database().ref('sensorLogs').on('value', function(snapshot){
    $('#sensorLogs').empty();
    var index = 0;

    snapshot.forEach(element => {
      var row = `<tr>
        <th scope="row">${++index}</th>
        <td>${element.val().date}</td>
        <td>${(element.val().confirmation == false ? "Onaylanmadi" : "Onaylandi")}</td>
        <td>${(element.val().confirmation == false ? `<button type="button" class="btn btn-primary" onClick="approveBreach(${index})">Onayla</button>` : "-")}</td>
        </tr>`;

      $('#sensorLogs').append(row);
    });
  });
}

function approveBreach(breachNo){
  //the approval state of a breach is changed here.
  var updates = {};
  updates['sensorLogs/securityBreach' + breachNo + '/confirmation'] = true;
  updates['approvedBreach'] = parseInt($('#approvedBreach').text()) + 1;
  firebase.database().ref().update(updates);
}

function exportTableToExcel(tableID, filename = ''){
  // from here, the table is downloaded as an excel table.
  var downloadLink;
  var dataType = 'application/vnd.ms-excel';
  var tableSelect = document.getElementById(tableID);
  var tableHTML = tableSelect.outerHTML.replace(/ /g, '%20');
  
  // Specify file name
  filename = filename?filename+'.xls':'excel_data.xls';
  
  // Create download link element
  downloadLink = document.createElement("a");
  
  document.body.appendChild(downloadLink);
  
  if(navigator.msSaveOrOpenBlob){
      var blob = new Blob(['\ufeff', tableHTML], {
          type: dataType
      });
      navigator.msSaveOrOpenBlob( blob, filename);
  }else{
      // Create a link to the file
      downloadLink.href = 'data:' + dataType + ', ' + tableHTML;
  
      // Setting the file name
      downloadLink.download = filename;
      
      //triggering the function
      downloadLink.click();
  }
}