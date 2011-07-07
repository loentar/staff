// synchronous example
function CalcAddSynch(nA, nB)
{
  var oCalcService = new samples.calc.CalcService();
  try
  {
    var nResult = oCalcService.Add(nA, nB);
    alert(nA + " + " + nB + " = " + nResult);
  }
  catch(oError)
  {
    alert("ERROR # " + oError.status + ":\n" + oError.responseText);
  }
}


// asynchronous example
function CalcAddAsynch(nA, nB)
{
  var oCalcService = new samples.calc.CalcService();

  function CalcAddResult(nResult)
  {
    alert(nA + " + " + nB + " = " + nResult);
  }

  function CalcAddError(oError)
  {
    alert("ERROR # " + oError.status + ":\n" + oError.responseText);
  }

  oCalcService.Add(nA, nB, CalcAddResult, CalcAddError);
}


// test loading of jquery, staff and client proxy. display the notice if some component was not loaded
setTimeout(function(){
  if (typeof jQuery == "undefined" || typeof staff == "undefined" || typeof samples == "undefined")
  {
    document.getElementById("idNotice").style.display = "";
  }
}, 2000);
