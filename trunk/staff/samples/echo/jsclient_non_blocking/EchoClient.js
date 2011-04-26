function EchoInvoke()
{
  var oEchoService = new Echo();
  oEchoService.EchoString("Hello, World!!",
  function(sResult)
  {
    alert("response from server: \"" + sResult + "\"");
  });
}
