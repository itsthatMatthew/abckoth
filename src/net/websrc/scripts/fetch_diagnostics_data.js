function getDiagnosticsData() {
  fetch('/getDiagnosticsData')
  .then(response => response.json())
  .then(jsonResponse => {
    var diag = document.getElementById('diagnostics-data');
    diag.innerHTML = '';
    appendData(diag, 'ESP-IDF Version', jsonResponse.esp_idf_version);
    appendData(diag, 'Arduino Core Version', jsonResponse.esp_arduino_core_version);
    appendData(diag, 'Model', jsonResponse.esp_model);
    appendData(diag, 'Cores', jsonResponse.esp_cores);
    appendData(diag, 'CPU Frequency', jsonResponse.esp_cpu_freq_mhz + ' MHz');
    appendData(diag, 'Flash Chip Speed', jsonResponse.esp_flash_chip_speed/1000000 + ' MHz');
    appendData(diag, 'Flash Chip Size', jsonResponse.esp_flash_chip_size/1048576 + ' MB');
    appendData(diag, 'Heap Size', jsonResponse.esp_heap_size.toLocaleString() + ' B');
    appendData(diag, 'Free Heap Size', jsonResponse.esp_free_heap.toLocaleString() + ' B');
    appendData(diag, 'Mac Address', jsonResponse.esp_mac_address.toString(16).match(/.{1,2}/g).reverse().join(':'));
  });
}

function appendData(diag, name, value) {
  var row = document.createElement('tr');
  row.innerHTML = '<td>' + name + '</td>' + '<td>'+ value + '</td>';
  diag.appendChild(row);
}

getDiagnosticsData();
setInterval(() => { getDiagnosticsData(); }, 10000);