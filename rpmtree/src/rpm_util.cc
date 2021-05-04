#include <string>

#include "rpm/rpmtd.h"
#include "rpm/rpmdb.h"


#include "rpm_util.h"

std::string RpmItem::getName(void)
{
  return name;
}

/* RpmUtil class */
std::string RpmUtil::getRpmNameBySofile(std::string fileName)
{
  return "";
}
