#ifndef _CHECKER_H_
#define _CHECKER_H_

#include <list>
#include <staff/common/IService.h>
#include "Ticket.h"

namespace samples
{
namespace sharedtypes
{
  //! ticket with used flag
  struct IssuedTicket: public ticket::Ticket
  {
    bool bUsed;  //!< is ticket was used
  };

  typedef std::list<IssuedTicket> IssuedTicketList; //!< issued ticket list

  //! checker: service to demonstrate how to use shared types
  class Checker: public staff::IService
  {
  public:
    //! virtual destructor
    virtual ~Checker() {}

    //! use ticket and mark it as used
    /*! \param stTicket - ticket
    */
    virtual void Use(const ticket::Ticket& stTicket) = 0;

    //! check ticket is issued and not used
    /*! \param stTicket - ticket
        \return true if ticket is issued and not used
      */
    virtual bool Check(const ticket::Ticket& stTicket) const = 0;

    //! get all ticket with flag "used"
    /*! \return tickets list with flag "used"
      */
    virtual IssuedTicketList GetAllTickets() const = 0;
  };
}
}

#endif // _CHECKER_H_

