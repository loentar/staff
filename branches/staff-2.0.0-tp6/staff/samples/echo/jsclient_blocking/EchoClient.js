function EchoInvoke()
{
  var oEchoService = new Echo();
  var sResult = oEchoService.EchoString("Hello, World!!");
  alert("response from server: \"" + sResult + "\"");
}
