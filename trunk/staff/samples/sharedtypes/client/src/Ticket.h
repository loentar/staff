#ifndef _TICKET_H_
#define _TICKET_H_

#include <string>
#include <list>

namespace samples
{
namespace ticket
{
  //! ticket
  struct Ticket
  {
    enum Type
    {
      Unknown,
      Primary,
      Reissued
    };

    // *defaultValue: Primary
    Type eType;                //!< ticket type
    // *defaultValue: 0
    int nId;                   //!< ticket id
    std::string sOwner;        //!< ticket owner

    Ticket():
      eType(Unknown), nId(0)
    {
    }
  };

  typedef std::list<Ticket> TicketsList; //!< tickets list
}
}

#endif // _TICKET_H_
