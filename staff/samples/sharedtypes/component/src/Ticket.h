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
    int nId;                   //!< ticket id
    std::string sOwner;        //!< ticket owner
  };

  typedef std::list<Ticket> TicketsList; //!< tickets list
}
}

#endif // _TICKET_H_
