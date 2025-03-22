fetch('/getAttributes')
  .then(response => response.json())
  .then(jsonResponse => {
    var table = document.getElementById('attributes-table');
    jsonResponse.forEach(element => {
        var row = document.createElement('tr');
        row.innerHTML = '<td>'+element.name+'</td>'+'<td>'+element.value+'</td>'+'<td>'+element.desc+'</td>';
        table.appendChild(row);
    }
    );
  }
);