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

#ifndef _SIGNALHANDLER_H_
#define _SIGNALHANDLER_H_

#include <rise/os/osdllexport.h>
#include <signal.h>
#include <rise/string/String.h>

namespace rise
{
  //!    обработчик сигнала
  class RISE_EXPORT CSignalHandler
  {
  public:
    enum ESIGNAL  //! список сигналов
    {
      ESIGNONE = 0, 
#ifdef WIN32
      //! в Win32 поддерживаются только
      //! ESIGNONE, ESIGINT, ESIGILL, ESIGABRT_COMPAT, ESIGFPE, ESIGSEGV, ESIGTERM, ESIGABRT
      //! остальные сигналы будут проигнорированы
      ESIGHUP, ESIGINT, ESIGQUIT, ESIGILL, ESIGTRAP, ESIGABRT_COMPAT, ESIGBUS, ESIGFPE,
      ESIGKILL, ESIGUSR1, ESIGSEGV, ESIGUSR2, ESIGPIPE, ESIGALRM, ESIGTERM, ESIGCHLD,
      ESIGCONT, ESIGSTOP, ESIGTSTP, ESIGTTIN, ESIGTTOU, ESIGABRT, ESIGURG, ESIGXCPU, ESIGXFSZ,
      ESIGVTALRM, ESIGPROF, ESIGWINCH, ESIGIO, ESIGPWR, ESIGSYS, ESIGRTMIN
#else
      ESIGHUP = SIGHUP,
      ESIGINT = SIGINT,
      ESIGQUIT = SIGQUIT,
      ESIGILL = SIGILL,
      ESIGTRAP = SIGTRAP,
      ESIGABRT = SIGABRT,
      ESIGBUS = SIGBUS,
      ESIGFPE = SIGFPE,
      ESIGKILL = SIGKILL,
      ESIGUSR1 = SIGUSR1,
      ESIGSEGV = SIGSEGV,
      ESIGUSR2 = SIGUSR2,
      ESIGPIPE = SIGPIPE,
      ESIGALRM = SIGALRM,
      ESIGTERM = SIGTERM,
      ESIGCHLD = SIGCHLD,
      ESIGCONT = SIGCONT,
      ESIGSTOP = SIGSTOP,
      ESIGTSTP = SIGTSTP,
      ESIGTTIN = SIGTTIN,
      ESIGTTOU = SIGTTOU,
      ESIGURG = SIGURG,
      ESIGXCPU = SIGXCPU,
      ESIGXFSZ = SIGXFSZ,
      ESIGVTALRM = SIGVTALRM,
      ESIGPROF = SIGPROF,
      ESIGWINCH = SIGWINCH,
      ESIGIO = SIGIO,
      ESIGPWR = SIGPWR,
      ESIGSYS = SIGSYS,
// mips hack
#ifdef __SIGRTMIN
      ESIGRTMIN = __SIGRTMIN
#else
      ESIGRTMIN = SIGRTMIN
#endif
#endif
    };
    
  public:
    virtual ~CSignalHandler();
    
    //!              подписка на сигнал nSignal
    /*  \param nSignal - номер сигнала
    */
    void RegisterSignal(int nSignal);

    //!              отписка от сигнала nSignal
    /*  \param nSignal - номер сигнала
    */
    void UnRegisterSignal(int nSignal);
    
    //!              отписка от всех сигналов
    void UnRegisterAll();
 
    //!              получить символическое описание сигнала
    /*  \param nSignal - номер сигнала
        \return символическое описание сигнала
    */
    static const CString& GetSignalStr(int nSignal);

  protected:
    //!               обработчик сигнала
    /*                эту функцию должен перекрыть потомок, чтобы обрабатывать сигналы
        \param nSignal - номер сигнала
    */
    virtual void SignalHandler(int nSignal) = 0;
  
    friend class CSignalCatcher;
  };
  
} // namespace rise

#endif // #ifndef _SIGNALCATCHER_H_
