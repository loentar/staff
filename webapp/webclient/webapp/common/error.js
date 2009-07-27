/*
 *  Copyright 2009 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

function GetErrorStr(nError)
{
  switch(nError)
  {
    case 200: return "The operation completed successfully";
    case 301: return "Page moved permanently";
    case 302: return "Page found";
    case 403: return "Forbidden";
    case 404: return "Page not found on server side";
    case 500: return "Internal Server Error";
    case 12001: return "No more handles could be generated at this time.";
    case 12002: return "The request has timed out.";
    case 12003: return "An extended error was returned from the server. This is typically a string or buffer containing a verbose error message. Call InternetGetLastResponseInfo to retrieve the error text.";
    case 12004: return "An internal error has occurred.";
    case 12005: return "The URL is invalid.";
    case 12006: return "The URL scheme could not be recognized or is not supported.";
    case 12007: return "The server name could not be resolved.";
    case 12008: return "The requested protocol could not be located.";
    case 12009: return "A request to InternetQueryOption or InternetSetOption specified an invalid option value.";
    case 12010: return "The length of an option supplied to InternetQueryOption or InternetSetOption is incorrect for the type of option specified.";
    case 12011: return "The request option cannot be set, only queried.";
    case 12012: return "The Win32 Internet function support is being shut down or unloaded.";
    case 12013: return "The request to connect and log on to an FTP server could not be completed because the supplied user name is incorrect.";
    case 12014: return "The request to connect and log on to an FTP server could not be completed because the supplied password is incorrect.";
    case 12015: return "The request to connect to and log on to an FTP server failed.";
    case 12016: return "The requested operation is invalid.";
    case 12017: return "The operation was canceled, usually because the handle on which the request was operating was closed before the operation completed.";
    case 12018: return "The type of handle supplied is incorrect for this operation.";
    case 12019: return "The requested operation cannot be carried out because the handle supplied is not in the correct state.";
    case 12020: return "The request cannot be made via a proxy.";
    case 12021: return "A required registry value could not be located.";
    case 12022: return "A required registry value was located but is an incorrect type or has an invalid value.";
    case 12023: return "Direct network access cannot be made at this time.";
    case 12024: return "An asynchronous request could not be made because a zero context value was supplied.";
    case 12025: return "An asynchronous request could not be made because a callback function has not been set.";
    case 12026: return "The required operation could not be completed because one or more requests are pending.";
    case 12027: return "The format of the request is invalid.";
    case 12028: return "The requested item could not be located.";
    case 12029: return "The attempt to connect to the server failed.";
    case 12030: return "The connection with the server has been terminated.";
    case 12031: return "The connection with the server has been reset.";
    case 12032: return "Calls for the Win32 Internet function to redo the request.";
    case 12033: return "The request to the proxy was invalid.";
    case 12036: return "The request failed because the handle already exists.";
    case 12037: return "SSL certificate date that was received from the server is bad. The certificate is expired.";
    case 12038: return "SSL certificate common name (host name field) is incorrect. For example, if you entered www.server.com and the common name on the certificate says www.different.com.";
    case 12039: return "The application is moving from a non-SSL to an SSL connection because of a redirect.";
    case 12040: return "The application is moving from an SSL to an non-SSL connection because of a redirect.";
    case 12041: return "Indicates that the content is not entirely secure. Some of the content being viewed may have come from unsecured servers.";
    case 12042: return "The application is posting and attempting to change multiple lines of text on a server that is not secure.";
    case 12043: return "The application is posting data to a server that is not secure.";
    case 12110: return "The requested operation cannot be made on the FTP session handle because an operation is already in progress.";
    case 12111: return "The FTP operation was not completed because the session was aborted.";
    case 12130: return "An error was detected while parsing data returned from the gopher server.";
    case 12131: return "The request must be made for a file locator.";
    case 12132: return "An error was detected while receiving data from the gopher server.";
    case 12133: return "The end of the data has been reached.";
    case 12134: return "The supplied locator is not valid.";
    case 12135: return "The type of the locator is not correct for this operation.";
    case 12136: return "The requested operation can only be made against a Gopher+ server or with a locator that specifies a Gopher+ operation.";
    case 12137: return "The requested attribute could not be located.";
    case 12138: return "The locator type is unknown.";
    case 12150: return "The requested header could not be located.";
    case 12151: return "The server did not return any headers.";
    case 12152: return "The server response could not be parsed.";
    case 12153: return "The supplied header is invalid.";
    case 12154: return "The request made to HttpQueryInfo is invalid.";
    case 12155: return "The header could not be added because it already exists.";
    case 12156: return "The redirection failed because either the scheme changed (for example, HTTP to FTP) or all attempts made to redirect failed (default is five attempts).";
  default:
    return "unknown error #" + nError;
 }
}
