_Notice: Staff samples should be installed._

## 1. Creating the client ##

Create the "calc" directory in apache2 htdocs dir (usualy located in `/var/www`);

Download jQuery script from http://jquery.com/ and put it as jquery.js into "calc" directory.

Copy staff.js from `staff/bindings/javascript/client` into "calc" directory.

Put the index.html page into directory "calc":
```
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

  <script type="text/javascript" src="jquery.js"></script>
  <script type="text/javascript" src="staff.js"></script>
  <script type="text/javascript" src="CalcService.js"></script>
  <script type="text/javascript" src="calc.js"></script>
</head>

<body>
  <div>
    a = <input id="a" value="1"></input><br/>
    b = <input id="b" value="2"></input><br/>
    <input type="button" value="add" onclick="javascript:CalcAdd($('#a')[0].value, $('#b')[0].value);"></input>
  </div>
</body>
</html>
```

Put the calc client source code as calc.js:
```
function CalcAdd(nA, nB)
{
  var oCalcService = new samples.calc.CalcService();
  var nResult = oCalcService.Add(nA, nB);
  alert(nA + " + " + nB + " = " + nResult);
}
```

## 2. Generating the proxy ##

Proxy can be generated from interface file and from the WSDL.

To generate proxy from interface file, please enter this command from `staff/samples/calc/component/src` directory:
```
staff_codegen -tjsclient CalcService.h
```

To generate proxy from WSDL, enter command as follows:
```
staff_codegen -pwsdl -tjsclient http://localhost:9090/axis2/services/staff.wsdl.Wsdl/get/samples.calc/samples.calc.CalcService.wsdl
```

Place generated CalcService.js into "calc" directory.

## 4. Testing ##

Open page in your web browser:

http://localhost/calc

To invoke the service input some numbers and click "add" button.

Message box with results should appear.
