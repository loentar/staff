# Introduction #

This document is a **draft** description of codegen's template language.


# Details #

## Accessors: ##
Start codegen with -x parameter to produce internal xml that represents current project structure. You could get access to xml elements by using `$(Node.Child)` operator.

Some examples:
  * `$(Project.Name)` - obtain name of project;
  * `$(Project.Interfaces.$Count)` - get count of project interfaces;
...

Special accessors list:
| **Name** |  **Description** |
|:---------|:-----------------|
| ThisElementName | returns name of current element |
| ThisElementValue | returns value of current element |
| ThisElementPath | returns full path of element in dot notation |
| Count | returns child count |
| Num | returns current element number by order |

## Variables ##

Declare a variable:
```
#var myvar 10
// get access to variable
myvar = $($myvar)
```

## Processing functions ##

To execute processing function append `.!functionname` to accessor or variable.

Example: convert interface's file name to lower case:
```
$(Interface.FileName.!tolower)
// "SomeFileName.H" -> "somefilename.h"
```

Processing functions list:
| **Name** |  **Description** |
|:---------|:-----------------|
| mangledot | replace dots to underscores `"." => "_"` |
| mangle | replace 2 colons to underscores `"::" => "_"` |
| dot | replace 2 colons to dots `"::" => "."`  |
| match/ | returns true if current context has substring |
| replace/ | replace matches in string to given value |
| replacenotof/ | replace all unmatched chars in string to given value |
| trim/ | remove leading and trailing symbols |
| trimleft/ | remove leading symbols |
| trimright/ | remove trailing symbols |
| trim | remove leading and trailing whitespaces |
| trimleft | remove leading whitespaces |
| trimright | remove trailing whitespaces |
| tolower | convert text to lower case |
| toupper | convert text to upper case |
| append/ | append a string |
| prepend/ | prepend a string |
| deprefix/ | remove prefix from string |
| depostfix/ | remove postfix from string |
| token/ | returns first token |
| lasttoken/ | returns last token |
| cut/ | cuts the string with given substring and gets first occurrence |
| cutlast/ | cuts the string with given substring and gets last occurrence |
| inc | increment number |
| dec | decrement number |
| add/ | add to number |
| sub/ | subtract from number |
| trunc | truncate float value to integer |


## Preprocessor blocks: ##

Short description of preprocessor blocks available:
| **Name** |  **Description**                              | **Example** |
|:---------|:----------------------------------------------|:------------|
| var    | declare a variable | `#var a this is a text variable`<br><code>#var b 123</code> <br>
<tr><td> ifeq<br>ifneq<br>else<br>ifeqend </td><td> Process conditional block </td><td> <code>#ifeq($($a),1)</code><br><code>code if variable a is equal 1</code><br><code>#else</code><br><code>code if variable a is not equal 1</code><br><code>#ifeqend</code><br> </td></tr>
<tr><td> foreach<br>end </td><td> process child nodes </td><td> <code>#foreach $(Interface.Structs)</code><br><code>Current struct is "$(.Namespace)$(.Name)"</code><br><code>#end</code></td></tr>
<tr><td> context<br>contextend </td><td> Switch current context to given child </td><td> <code>#context $(Project.Interfaces.Interface.Structs)</code><br><code>count of structs in first interface = $(.$Count)</code><br><code>#contextend</code> </td></tr>
<tr><td> cginclude </td><td> include given file </td><td> <code>#cginclude "somefile"</code> </td></tr>
<tr><td> indent </td><td> set indentation </td><td> <code>#indent +2</code> </td></tr>
<tr><td> fileopen </td><td> create a file and put all output into that file </td><td> <code>#fileopen my.cpp</code> </td></tr>
<tr><td> fileclose </td><td> close previously opened file </td><td> <code>#fileclose</code> </td></tr>
<tr><td> mkdir </td><td> create a directory </td><td> <code>#mkdir src</code> </td></tr>
<tr><td> cgwarning </td><td> write warning message </td><td> <code>#cgwarning invalid parameter</code> </td></tr>
<tr><td> cgerror </td><td> write error message and stop generation </td><td> <code>#cgerror some critical error description</code> </td></tr>
<tr><td> cgpushvars<br>cgpopvars </td><td> push or pop variables into/from stack </td><td>  </td></tr>
<tr><td> cgdumpvars </td><td> output current variables with values (debug purposes only) </td><td> <code>#cgdumpvars</code> </td></tr></tbody></table>



<h2>Complete example</h2>

TODO