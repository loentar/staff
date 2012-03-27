#ifndef _ISSUER_H_
#define _ISSUER_H_

#include <staff/common/IService.h>
#include "Ticket.h"

namespace samples
{
namespace sharedtypes
{
  //! Issuer: service to demonstrate how to use shared types
  class Issuer: public staff::IService
  {
  public:
    //! virtual destructor
    virtual ~Issuer() {}

    //! issue new ticket
    /*! \param sOwner - ticket owner
        \return new issued ticket
     */
    virtual ticket::Ticket Issue(const std::string& sOwner) = 0;

    //! check is ticked was issued
    /*! \param stTicket - ticket to check
        \return true if ticked was issued
      */
    virtual bool WasIssued(const ticket::Ticket& stTicket) = 0;

    //! get all issued tickets
    /*! \return list of issued tickets
      */
    virtual ticket::TicketsList GetAllTickets() const = 0;
  };
}
}

#endif // _ISSUER_H_

