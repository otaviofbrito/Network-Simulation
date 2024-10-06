function calcular_rho() {
  // Obtendo os valores dos inputs
  const lambda = parseFloat(document.getElementById('ocup_lambda').value)
  const L = parseFloat(document.getElementById('ocup_l').value)
  const R = parseFloat(document.getElementById('ocup_r').value)
  // Verificando se todos os valores foram inseridos
  if (isNaN(lambda) || isNaN(L) || isNaN(R)) {
    document.getElementById('ocup_res').innerText = ' = Inválido!'
    return
  }

  // Calculando a ocupação
  const ocupacaoCalculada = (((lambda * L) / R) * 100).toFixed(2)

  // Exibindo o resultado na página
  document.getElementById('ocup_res').innerText = ` = ${ocupacaoCalculada}%`
}

function calcular_EN() {
  const rho = parseFloat(document.getElementById('e_rho').value)
  if (isNaN(rho)) {
    document.getElementById('e_res').innerText = ' = Inválido!'
    return
  }
  const e_n = (rho / (1 - rho)).toFixed(3)
  document.getElementById('e_res').innerText = ` = ${e_n}`
}

function calcular_EW() {
  const lambda = parseFloat(document.getElementById('e_lambda').value)
  const L = parseFloat(document.getElementById('e_l').value)
  const R = parseFloat(document.getElementById('e_r').value)

  if (isNaN(lambda) || isNaN(L) || isNaN(R)) {
    document.getElementById('w_res').innerText = ' = Inválido!'
    return
  }
  const e_w = (L / (R - L * lambda)).toFixed(3)
  document.getElementById('w_res').innerText = ` = ${e_w}`
}
