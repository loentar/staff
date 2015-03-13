# Summary #

This document is an overview of codegenerators provided by open source Web Services Frameworks in contrast to WSF Staff.

## Command lines to generate complete client from WSDL ##

| **WSF Staff**    | `staff_codegen -pwsdl -tclient_all Calculator.wsdl` |
|:-----------------|:----------------------------------------------------|
| **Axis2/C**      | `$AXIS2_HOME/bin/WSDL2C.sh -uri Calculator.wsdl -u -uw` |
| **WSF C++ WSO2** | `$WSO2_HOME/bin/tools/codegen/wsdl2cpp/WSDL2CPP.sh -uri Calculator.wsdl -u -uw` |
| **gSoap**        | `wsdl2h Calculator.wsdl<br>soapcpp2 Calculator.h -I/usr/include/gsoap` |

## Generated files list ##

| **WSF Staff**        | **Axis2/C**      | **WSF C++ WSO2** | **gSoap**        |
|:---------------------|:-----------------|:-----------------|:-----------------|
| src/Calculator.h<br>src/CalculatorProxy.h <font color='red' size='2'><code>*</code></font><br>src/CalculatorProxy.cpp <font color='red' size='2'><code>*</code></font><br>src/main.cpp<br>Makefile<br>codegen.cmd<br>Calculator.vcproj<br>Calculator.pro <table><thead><th> src/adb_add.c<br>src/adb_add.h<br>src/adb_addResponse.c<br>src/adb_addResponse.h<br>src/adb_div.c<br>src/adb_div.h<br>src/adb_divResponse.c<br>src/adb_divResponse.h<br>src/adb_mul.c<br>src/adb_mul.h<br>src/adb_mulResponse.c<br>src/adb_mulResponse.h<br>src/adb_sub.c<br>src/adb_sub.h<br>src/adb_subResponse.c<br>src/adb_subResponse.h<br>src/axis2_extension_mapper.c<br>src/axis2_extension_mapper.h<br>src/axis2_stub_Calculator.c<br>src/axis2_stub_Calculator.h<br>src/CalculatorClient.vcproj </th><th> src/Add.cpp<br>src/Add.h<br>src/AddResponse.cpp<br>src/AddResponse.h<br>src/CalculatorClient.vcproj<br>src/CalculatorStub.cpp<br>src/CalculatorStub.h<br>src/Div.cpp<br>src/Div.h<br>src/DivResponse.cpp<br>src/DivResponse.h<br>src/ICalculatorCallback.h<br>src/Mul.cpp<br>src/Mul.h<br>src/MulResponse.cpp<br>src/MulResponse.h<br>src/Sub.cpp<br>src/Sub.h<br>src/SubResponse.cpp<br>src/SubResponse.h </th><th> Calculator.h<br>CalculatorSoapBinding.add.req.xml<br>CalculatorSoapBinding.add.res.xml<br>CalculatorSoapBinding.div.req.xml<br>CalculatorSoapBinding.div.res.xml<br>CalculatorSoapBinding.mul.req.xml<br>CalculatorSoapBinding.mul.res.xml<br>CalculatorSoapBinding.nsmap<br>CalculatorSoapBinding.sub.req.xml<br>CalculatorSoapBinding.sub.res.xml<br>soapCalculatorSoapBindingObject.h<br>soapCalculatorSoapBindingProxy.h<br>soapC.cpp<br>soapClient.cpp<br>soapClientLib.cpp<br>soapH.h<br>soapServer.cpp<br>soapServerLib.cpp<br>soapStub.h<br> </th></thead><tbody></tbody></table>

<font color='red' size='2'><code>*</code></font> files will be generated during build process.<br>
<br>
<h2>Command lines to compile client using GCC</h2>

<table><thead><th> <b>WSF Staff</b>        </th><th> <font color='green' size='1'>using GNU Make:</font><br><code>make</code><br /><font color='green' size='1'><b>or</b> using shadow build with qmake and GNU Make:</font><br><code>mkdir -p ../calculator-shadow &amp;&amp; cd ../calculator-shadow &amp;&amp; qmake ../calculator &amp;&amp; make</code> </th></thead><tbody>
<tr><td> <b>Axis2/C</b>      </td><td> <font color='green' size='1'>Makefile is not provided. Command line to compile client:</font><br><code>gcc -o calculator_client src/*.c -I$AXIS2C_HOME/include/axis2-1.6.0 -L$AXIS2C_HOME/lib -laxutil -laxis2_axiom -laxis2_parser -laxis2_engine -lpthread -laxis2_http_sender -laxis2_http_receiver -ldl -Wl,--rpath -Wl,$AXIS2C_HOME/lib</code> </td></tr>
<tr><td> <b>WSF C++ WSO2</b> </td><td> <font color='green' size='1'>Makefile is not provided. Command line to compile client:</font><br><code>g++ -o calculator_client src/*.cpp -I$AXIS2C_HOME/include/axis2-1.6.0 -L$AXIS2C_HOME/lib -IWSFCPP_HOME/include -I$WSFCPP_HOME/include/axis2-1.6.0/platforms/ -Isrc -laxutil -laxis2_axiom -laxis2_parser -laxis2_engine -lpthread -laxis2_http_sender -laxis2_http_receiver -ldl -lwso2_wsf -Wl,--rpath -Wl,$AXIS2C_HOME/lib -Wl,--rpath -Wl,$WSFCPP_HOME/lib</code> </td></tr>
<tr><td> <b>gSoap</b>        </td><td> <font color='green' size='1'>Makefile is not provided. Command line to compile client:</font><br><code>g++ calculator_client.cpp soapClient.cpp soapC.cpp /usr/include/gsoap/stdsoap2.cpp -o calculator_client -I/usr/include/gsoap</code> </td></tr></tbody></table>


<h2>Codegenerator capabilities</h2>

<table><thead><th> <b>Criteria</b>                           </th><th> <b>WSF Staff</b> </th><th> <b>Axis2/C</b> </th><th> <b>WSF C++ WSO2</b> </th><th> <b>gSoap</b> </th></thead><tbody>
<tr><td> can generate .vcproj                 </td><td> yes         </td><td> yes       </td><td> yes            </td><td> no      </td></tr>
<tr><td> can generate Makefile for GNU Make   </td><td> yes         </td><td> no        </td><td> no             </td><td> no      </td></tr>
<tr><td> can generate .pro file for qmake     </td><td> yes         </td><td> no        </td><td> no             </td><td> no      </td></tr>
<tr><td> can generate example of client impl. </td><td> yes         </td><td> no        </td><td> no             </td><td> no      </td></tr>
<tr><td> codegeneration time                  </td><td> 18 ms       </td><td> ~1800 ms  </td><td> ~1800 ms       </td><td> 16 ms   </td></tr></tbody></table>

<h2>WSDL/SOAP Support</h2>

<i>Table below is in unwrapped mode of Axis2/C and WSO2 codegenerators.</i>

<table><thead><th> <b>Support</b>                           </th><th> <b>WSF Staff</b>   </th><th> <b>Axis2/C</b> </th><th> <b>WSF C++ WSO2</b> </th><th> <b>gSoap</b> </th></thead><tbody>
<tr><td> Custom OM handling (xsd:any)        </td><td> yes           </td><td> no (bug?) </td><td> yes            </td><td> yes     </td></tr>
<tr><td> Attributes in structures            </td><td> yes           </td><td> yes       </td><td> yes            </td><td> yes     </td></tr>
<tr><td> Attributes in requests              </td><td> yes           </td><td> yes       </td><td> yes            </td><td> yes<code>*</code>  </td></tr>
<tr><td> Attributes in responses             </td><td> yes           </td><td> no (bug?) </td><td> no (bug?)      </td><td> yes<code>*</code>  </td></tr>
<tr><td> Base64Binary                       </td><td> yes           </td><td> yes       </td><td> yes<code>**</code>        </td><td> yes     </td></tr>
<tr><td> HexBinary                          </td><td> yes           </td><td> no        </td><td> no             </td><td> yes     </td></tr>
<tr><td> Optional types                      </td><td> yes           </td><td> yes       </td><td> yes            </td><td> yes     </td></tr>
<tr><td> Nillable types                      </td><td> yes           </td><td> yes       </td><td> yes            </td><td> yes     </td></tr>
<tr><td> Choice                              </td><td> yes           </td><td> yes       </td><td> yes            </td><td> yes     </td></tr>
<tr><td> AttributeGroup                     </td><td> yes           </td><td> yes       </td><td> yes            </td><td> yes     </td></tr>
<tr><td> Dynamically typed data              </td><td> yes           </td><td> no (bug?) </td><td> no             </td><td> yes     </td></tr>
<tr><td> SOAP Arrays                         </td><td> yes           </td><td> partial   </td><td> partial        </td><td> yes     </td></tr>
<tr><td> Asynchronous service invocation     </td><td> yes           </td><td> yes       </td><td> yes            </td><td> yes     </td></tr></tbody></table>

<code>*</code> unwrapping is not supported<br>
<br>
<code>**</code> C++ support is missing: WSO2 using Axis2/C <code>axutil_base64_binary_t</code> to represent base64Binary<br>
<br>
<br>
<h2>Example of code produced by frameworks</h2>

As source we take very simple WSDL:<br>
<pre><code>&lt;?xml version="1.0" encoding="UTF-8" ?&gt;<br>
&lt;wsdl:definitions <br>
    xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"<br>
    xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/"<br>
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"<br>
    xmlns:tns="http://tempui.org/echo"<br>
    targetNamespace="http://tempui.org/echo"&gt;<br>
<br>
  &lt;wsdl:message name="echoStringRequest"&gt;<br>
    &lt;wsdl:part name="data" type="xsd:string"/&gt;<br>
  &lt;/wsdl:message&gt;<br>
  &lt;wsdl:message name="echoStringResponse"&gt;<br>
    &lt;wsdl:part name="echoStringReturn" type="xsd:string"/&gt;<br>
  &lt;/wsdl:message&gt;<br>
<br>
  &lt;wsdl:portType name="echoPortType"&gt;<br>
    &lt;wsdl:operation name="echoString"&gt;<br>
      &lt;wsdl:input message="tns:echoStringRequest"/&gt;<br>
      &lt;wsdl:output message="tns:echoStringResponse"/&gt;<br>
    &lt;/wsdl:operation&gt;<br>
  &lt;/wsdl:portType&gt;<br>
<br>
  &lt;wsdl:binding name="echoBinding" type="tns:echoPortType"&gt;<br>
    &lt;soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/&gt;<br>
    &lt;wsdl:operation name="echoString"&gt;<br>
      &lt;soap:operation style="rpc"/&gt;<br>
      &lt;wsdl:input&gt;<br>
        &lt;soap:body use="literal" namespace="http://tempui.org/echo"/&gt;<br>
      &lt;/wsdl:input&gt;<br>
      &lt;wsdl:output&gt;<br>
        &lt;soap:body use="literal" namespace="http://tempui.org/echo"/&gt;<br>
      &lt;/wsdl:output&gt;<br>
    &lt;/wsdl:operation&gt;<br>
  &lt;/wsdl:binding&gt;<br>
<br>
  &lt;wsdl:service name="echo"&gt;<br>
    &lt;wsdl:port name="echoPort" binding="tns:echoBinding"&gt;<br>
      &lt;soap:address location="http://localhost/axis2/services/echo"/&gt;<br>
    &lt;/wsdl:port&gt;<br>
  &lt;/wsdl:service&gt;<br>
&lt;/wsdl:definitions&gt;<br>
</code></pre>


<h3>WSF Staff</h3>

Files produced:<br>
<pre><code>Makefile<br>
echo.pro<br>
echo.vcproj<br>
codegen.cmd<br>
src/echo.h<br>
src/main.cpp<br>
</code></pre>

<h4>Service interface</h4>
<pre><code>// This file generated by staff_codegen<br>
// For more information please visit: http://code.google.com/p/staff/<br>
<br>
#ifndef _echo_h_<br>
#define _echo_h_<br>
<br>
#include &lt;string&gt;<br>
#include &lt;staff/common/IService.h&gt;<br>
<br>
  //!  service echo<br>
  // *serviceUri: http://localhost/axis2/services/echo<br>
  // *soapVersion: 1.1<br>
  class echo: public staff::IService<br>
  {<br>
  public:<br>
    // *responseElement: echoStringResponse<br>
    // *resultElement: echoStringReturn<br>
    virtual std::string echoString(const std::string&amp; data) = 0;<br>
  };<br>
<br>
<br>
#endif // _echo_h_<br>
</code></pre>

<h4>Client implementation</h4>
<pre><code>// This file generated by staff_codegen<br>
// For more information please visit: http://code.google.com/p/staff/<br>
// Client skeleton<br>
<br>
#include &lt;memory&gt;<br>
#include &lt;staff/utils/Log.h&gt;<br>
#include &lt;staff/common/logoperators.h&gt;<br>
#include &lt;staff/common/Exception.h&gt;<br>
#include &lt;staff/client/ServiceFactory.h&gt;<br>
#include "echo.h"<br>
<br>
<br>
int main(int /*nArgs*/, const char* /*paszArgs*/[])<br>
{<br>
  try<br>
  {<br>
    std::auto_ptr&lt; ::echo &gt; pecho(::staff::ServiceFactory::Inst().GetService&lt; ::echo &gt;());<br>
<br>
    STAFF_ASSERT(pecho.get(), "Cannot get client for service echo!");<br>
<br>
    // Invoke Your service here:<br>
<br>
    // std::string data;<br>
    // const std::string&amp; echoStringResponse = pecho-&gt;echoString(data);<br>
    // staff::LogInfo() &lt;&lt; "echoString result: " &lt;&lt; echoStringResponse;<br>
  }<br>
  STAFF_CATCH_ALL<br>
<br>
  return 0;<br>
}<br>
</code></pre>

<h3>Axis2/C</h3>

Files produced:<br>
<pre><code>axis2_extension_mapper.h<br>
echoClient.vcproj<br>
adb_echoStringResponse.h<br>
adb_echoString.h<br>
adb_echoStringResponse.c<br>
adb_echoString.c<br>
axis2_stub_echo.c<br>
axis2_stub_echo.h<br>
axis2_extension_mapper.c<br>
</code></pre>

<h4>Client stub header</h4>

<pre><code><br>
<br>
        /**<br>
        * axis2_stub_echo.h<br>
        *<br>
        * This file was auto-generated from WSDL for "echo|http://tempui.org/echo" service<br>
        * by the Apache Axis2/Java version: 1.6.0  Built on : May 17, 2011 (04:19:43 IST)<br>
        */<br>
<br>
        #include &lt;stdio.h&gt;<br>
        #include &lt;axiom.h&gt;<br>
        #include &lt;axutil_utils.h&gt;<br>
        #include &lt;axiom_soap.h&gt;<br>
        #include &lt;axis2_client.h&gt;<br>
        #include &lt;axis2_stub.h&gt;<br>
<br>
       <br>
         #include "adb_echoString.h"<br>
        <br>
         #include "adb_echoStringResponse.h"<br>
        <br>
<br>
	#ifdef __cplusplus<br>
	extern "C" {<br>
	#endif<br>
<br>
        /***************** function prototypes - for header file *************/<br>
        /**<br>
         * axis2_stub_create_echo<br>
         * Create and return the stub with services populated<br>
         * @param env Environment ( mandatory)<br>
         * @param client_home Axis2/C home ( mandatory )<br>
         * @param endpoint_uri Service endpoint uri( optional ) - if NULL default picked from WSDL used<br>
         * @return Newly created stub object<br>
         */<br>
        axis2_stub_t* AXIS2_CALL<br>
        axis2_stub_create_echo(const axutil_env_t *env,<br>
                                        const axis2_char_t *client_home,<br>
                                        const axis2_char_t *endpoint_uri);<br>
        /**<br>
         * axis2_stub_populate_services_for_echo<br>
         * populate the svc in stub with the service and operations<br>
         * @param stub The stub<br>
         * @param env environment ( mandatory)<br>
         */<br>
        void AXIS2_CALL <br>
        axis2_stub_populate_services_for_echo( axis2_stub_t *stub, const axutil_env_t *env);<br>
        /**<br>
         * axis2_stub_get_endpoint_uri_of_echo<br>
         * Return the endpoint URI picked from WSDL<br>
         * @param env environment ( mandatory)<br>
         * @return The endpoint picked from WSDL<br>
         */<br>
        axis2_char_t* AXIS2_CALL<br>
        axis2_stub_get_endpoint_uri_of_echo(const axutil_env_t *env);<br>
<br>
<br>
        <br>
            /**<br>
             * Auto generated function declaration<br>
             * for "echoString|http://tempui.org/echo" operation.<br>
             * @param stub The stub (axis2_stub_t)<br>
             * @param env environment ( mandatory)<br>
             * @param _data of the axis2_char_t*<br>
             *<br>
             * @return axis2_char_t*<br>
             */<br>
<br>
            axis2_char_t* AXIS2_CALL <br>
            axis2_stub_op_echo_echoString( axis2_stub_t *stub, const axutil_env_t *env,<br>
                                                  axis2_char_t* _data);<br>
          <br>
<br>
        /**<br>
         * Auto generated function for asynchronous invocations<br>
         * for "echoString|http://tempui.org/echo" operation.<br>
         * @param stub The stub<br>
         * @param env environment ( mandatory)<br>
         * @param _data of the axis2_char_t*<br>
         * @param user_data user data to be accessed by the callbacks<br>
         * @param on_complete callback to handle on complete<br>
         * @param on_error callback to handle on error<br>
         */<br>
<br>
<br>
        void AXIS2_CALL<br>
        axis2_stub_start_op_echo_echoString( axis2_stub_t *stub, const axutil_env_t *env,<br>
                                                  axis2_char_t* _data,<br>
                                                  void *user_data,<br>
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, axis2_char_t* _echoStringResponse, void *data),<br>
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );<br>
<br>
        <br>
<br>
<br>
    /** we have to reserve some error codes for adb and for custom messages */<br>
    #define AXIS2_STUB_ECHO_ERROR_CODES_START (AXIS2_ERROR_LAST + 2000)<br>
<br>
    typedef enum <br>
    {<br>
        AXIS2_STUB_ECHO_ERROR_NONE = AXIS2_STUB_ECHO_ERROR_CODES_START,<br>
        <br>
        AXIS2_STUB_ECHO_ERROR_LAST<br>
    } axis2_stub_echo_error_codes;<br>
<br>
	#ifdef __cplusplus<br>
	}<br>
	#endif<br>
   <br>
<br>
<br>
</code></pre>



<h3>WSF C++ WSO2</h3>

Files produced:<br>
<pre><code>EchoString.h<br>
IechoCallback.h<br>
EchoStringResponse.h<br>
EchoStringResponse.cpp<br>
echoClient.vcproj<br>
echoStub.h<br>
echoStub.cpp<br>
EchoString.cpp<br>
</code></pre>


<h4>Client stub header</h4>

<pre><code><br>
<br>
#ifndef ECHOSTUB_H<br>
#define ECHOSTUB_H<br>
/**<br>
* echoStub.h<br>
*<br>
* This file was auto-generated from WSDL for "echo|http://tempui.org/echo" service<br>
* by the Apache Axis2/Java version: 1.0  Built on : Oct 15, 2009 (12:36:59 IST)<br>
*/<br>
<br>
#include &lt;stdio.h&gt;<br>
#include &lt;OMElement.h&gt;<br>
#include &lt;Stub.h&gt;<br>
#include &lt;ServiceClient.h&gt;<br>
<br>
<br>
#include &lt;EchoString.h&gt;<br>
<br>
#include &lt;EchoStringResponse.h&gt;<br>
<br>
<br>
namespace org_tempui_echo<br>
{<br>
<br>
#define ECHOSTUB_ERROR_CODES_START (AXIS2_ERROR_LAST + 2000)<br>
<br>
typedef enum<br>
{<br>
     ECHOSTUB_ERROR_NONE = ECHOSTUB_ERROR_CODES_START,<br>
<br>
    ECHOSTUB_ERROR_LAST<br>
} echoStub_error_codes;<br>
<br>
 class IechoCallback;<br>
<br>
 <br>
<br>
class echoStub : public wso2wsf::Stub<br>
{<br>
<br>
        public:<br>
        /**<br>
         *  Constructor of echo class<br>
         *  @param client_home WSF/C home directory<br>
         *  <br>
         */<br>
        echoStub(std::string&amp; client_home);<br>
<br>
        /**<br>
         *  Constructor of echo class<br>
         *  @param client_home WSF/C home directory<br>
         *  @param endpoint_uri The to endpoint uri,<br>
         */<br>
<br>
        echoStub(std::string&amp; client_home, std::string&amp; endpoint_uri);<br>
<br>
        /**<br>
         * Populate Services for echoStub<br>
         */<br>
        void WSF_CALL<br>
        populateServicesForecho();<br>
<br>
        /**<br>
         * Get the endpoint uri of the echoStub<br>
         */<br>
<br>
        std::string WSF_CALL<br>
        getEndpointUriOfecho();<br>
<br>
        <br>
<br>
            /**<br>
             * Auto generated function declaration<br>
             * for "echoString|http://tempui.org/echo" operation.<br>
             * <br>
             * @param _data of the std::string<br>
             *<br>
             * @return std::string<br>
             */<br>
<br>
            std::string WSF_CALL echoString( std::string _data);<br>
          <br>
<br>
        /**<br>
         * Auto generated function for asynchronous invocations<br>
         * for "echoString|http://tempui.org/echo" operation.<br>
         * @param stub The stub<br>
         * <br>
         * @param _data of the std::string<br>
         * @param ICallback callback handler<br>
         */<br>
<br>
<br>
        void WSF_CALL<br>
        start_echoString(std::string _data,IechoCallback* callback);<br>
<br>
          <br>
<br>
<br>
};<br>
<br>
/** we have to reserve some error codes for adb and for custom messages */<br>
<br>
<br>
<br>
}<br>
<br>
<br>
        <br>
#endif        <br>
   <br>
<br>
<br>
</code></pre>




<h3>gSoap</h3>

Files produced:<br>
<pre><code>soapC.cpp<br>
echoBinding.echoString.res.xml<br>
echoBinding.nsmap<br>
soapechoBindingProxy.h<br>
echoBinding.echoString.req.xml<br>
echo.h<br>
soapH.h<br>
soapStub.h<br>
soapServerLib.cpp<br>
soapClient.cpp<br>
soapServer.cpp<br>
echo.wsdl<br>
soapClientLib.cpp<br>
soapechoBindingObject.h<br>
</code></pre>


<h4>Client stub header</h4>

<pre><code>/* soapStub.h<br>
   Generated by gSOAP 2.7.9l from echo.h<br>
   Copyright(C) 2000-2007, Robert van Engelen, Genivia Inc. All Rights Reserved.<br>
   This part of the software is released under one of the following licenses:<br>
   GPL, the gSOAP public license, or Genivia's license for commercial use.<br>
*/<br>
<br>
#ifndef soapStub_H<br>
#define soapStub_H<br>
#include &lt;vector&gt;<br>
#include "stdsoap2.h"<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Enumerations                                                               *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Classes and Structs                                                        *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
<br>
#if 0 /* volatile type: do not redeclare */<br>
<br>
#endif<br>
<br>
#ifndef SOAP_TYPE_ns1__echoStringResponse<br>
#define SOAP_TYPE_ns1__echoStringResponse (7)<br>
/* ns1:echoStringResponse */<br>
struct ns1__echoStringResponse<br>
{<br>
public:<br>
	std::string echoStringReturn;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */<br>
};<br>
#endif<br>
<br>
#ifndef SOAP_TYPE_ns1__echoString<br>
#define SOAP_TYPE_ns1__echoString (11)<br>
/* ns1:echoString */<br>
struct ns1__echoString<br>
{<br>
public:<br>
	std::string data;	/* required element of type xsd:string */<br>
};<br>
#endif<br>
<br>
#ifndef SOAP_TYPE_SOAP_ENV__Header<br>
#define SOAP_TYPE_SOAP_ENV__Header (12)<br>
/* SOAP Header: */<br>
struct SOAP_ENV__Header<br>
{<br>
#ifdef WITH_NOEMPTYSTRUCT<br>
private:<br>
	char dummy;	/* dummy member to enable compilation */<br>
#endif<br>
};<br>
#endif<br>
<br>
#ifndef SOAP_TYPE_SOAP_ENV__Code<br>
#define SOAP_TYPE_SOAP_ENV__Code (13)<br>
/* SOAP Fault Code: */<br>
struct SOAP_ENV__Code<br>
{<br>
public:<br>
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */<br>
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */<br>
};<br>
#endif<br>
<br>
#ifndef SOAP_TYPE_SOAP_ENV__Detail<br>
#define SOAP_TYPE_SOAP_ENV__Detail (15)<br>
/* SOAP-ENV:Detail */<br>
struct SOAP_ENV__Detail<br>
{<br>
public:<br>
	int __type;	/* any type of element &lt;fault&gt; (defined below) */<br>
	void *fault;	/* transient */<br>
	char *__any;<br>
};<br>
#endif<br>
<br>
#ifndef SOAP_TYPE_SOAP_ENV__Reason<br>
#define SOAP_TYPE_SOAP_ENV__Reason (18)<br>
/* SOAP-ENV:Reason */<br>
struct SOAP_ENV__Reason<br>
{<br>
public:<br>
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */<br>
};<br>
#endif<br>
<br>
#ifndef SOAP_TYPE_SOAP_ENV__Fault<br>
#define SOAP_TYPE_SOAP_ENV__Fault (19)<br>
/* SOAP Fault: */<br>
struct SOAP_ENV__Fault<br>
{<br>
public:<br>
	char *faultcode;	/* optional element of type xsd:QName */<br>
	char *faultstring;	/* optional element of type xsd:string */<br>
	char *faultactor;	/* optional element of type xsd:string */<br>
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */<br>
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */<br>
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */<br>
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */<br>
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */<br>
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */<br>
};<br>
#endif<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Types with Custom Serializers                                              *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Typedefs                                                                   *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
#ifndef SOAP_TYPE__XML<br>
#define SOAP_TYPE__XML (4)<br>
typedef char *_XML;<br>
#endif<br>
<br>
#ifndef SOAP_TYPE__QName<br>
#define SOAP_TYPE__QName (5)<br>
typedef char *_QName;<br>
#endif<br>
<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Typedef Synonyms                                                           *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Externals                                                                  *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Service Operations                                                         *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
<br>
SOAP_FMAC5 int SOAP_FMAC6 ns1__echoString(struct soap*, std::string data, struct ns1__echoStringResponse &amp;_param_1);<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Stubs                                                                      *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
<br>
SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__echoString(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string data, struct ns1__echoStringResponse &amp;_param_1);<br>
<br>
/******************************************************************************\<br>
 *                                                                            *<br>
 * Skeletons                                                                  *<br>
 *                                                                            *<br>
\******************************************************************************/<br>
<br>
SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);<br>
<br>
SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);<br>
<br>
SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns1__echoString(struct soap*);<br>
<br>
#endif<br>
<br>
/* End of soapStub.h */<br>
</code></pre>



