var database;
var isSystemOpened;

$(document).ready(function () {
  firebase.initializeApp(firebaseConfig);
  database = firebase.database(); 
  checkSystem(); 
  checkNumberApprovedBreach();
  checkNumberBreach();
  systemOpenClose();
  checkBreachList();
});

function checkSystem() {
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

function checkNumberBreach(){
  firebase.database().ref('breachNo').on('value', function(snapshot){
    $('#breachNo').text(snapshot.val());
    $('#unapprovedBreach').text(snapshot.val() - parseInt($('#approvedBreach').text()));
  });
}

function checkNumberApprovedBreach(){
  firebase.database().ref('approvedBreach').on('value', function(snapshot){
    $('#approvedBreach').text(snapshot.val());
    $('#unapprovedBreach').text(parseInt($('#breachNo').text()) - snapshot.val());
  });
}

function systemOpenClose(){
  $('#systemOpenClose').on('click',function(){
    var updates = {};
    updates['/isSystemOpened'] = isSystemOpened == 0 ? 1 : 0;
    firebase.database().ref().update(updates);
  })
}

function checkBreachList(){
  firebase.database().ref('sensorLogs').on('value', function(snapshot){
    $('#sensorLogs').empty();
    var index = 0;

    snapshot.forEach(element => {
      var satir = `<tr>
        <th scope="row">${++index}</th>
        <td>${element.val().date}</td>
        <td>${(element.val().confirmation == false ? "Onaylanmadi" : "Onaylandi")}</td>
        <td>${(element.val().confirmation == false ? `<button type="button" class="btn btn-primary" onClick="approveBreach(${index})">Onayla</button>` : "")}</td>
        </tr>`;

      $('#sensorLogs').append(satir);
    });
  });
}

function approveBreach(ihlalNo){
  var updates = {};
  updates['sensorLogs/securityBreach' + ihlalNo + '/confirmation'] = true;
  updates['approvedBreach'] = parseInt($('#approvedBreach').text()) + 1;
  firebase.database().ref().update(updates);
}