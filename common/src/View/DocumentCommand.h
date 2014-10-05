/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__DocumentCommand__
#define __TrenchBroom__DocumentCommand__

#include "View/UndoableCommand.h"

namespace TrenchBroom {
    namespace View {
        class DocumentCommand : public UndoableCommand {
        private:
            size_t m_modificationCount;
        public:
            DocumentCommand(CommandType type, const String& name);
            virtual ~DocumentCommand();
        public:
            bool performDo(MapDocumentCommandFacade* document);
            bool performUndo(MapDocumentCommandFacade* document);
            bool collateWith(UndoableCommand* command);
        private:
            size_t documentModificationCount() const;
        };
    }
}

#endif /* defined(__TrenchBroom__DocumentCommand__) */
