/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KREPORTPRINTRENDERER_H
#define KREPORTPRINTRENDERER_H

#include "KReportRendererBase.h"

class ORODocument;

namespace KReportPrivate {
  
class PrintRenderer : public KReportRendererBase
{
    public:
        PrintRenderer();
        ~PrintRenderer() override;
        bool render(const KReportRendererContext&, ORODocument*, int page = -1) override;

    private:
        bool setupPrinter(ORODocument *, QPrinter *);
};

}
#endif // KREPORTPRINTRENDERER_H



