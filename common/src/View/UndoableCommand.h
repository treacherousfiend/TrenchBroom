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

#ifndef __TrenchBroom__UndoableCommand__
#define __TrenchBroom__UndoableCommand__

#include "View/Command.h"

namespace TrenchBroom {
    namespace View {
        class MapDocumentCommandFacade;
        
        class UndoableCommand : public Command {
        public:
            UndoableCommand(CommandType type, const String& name);
            virtual ~UndoableCommand();

            virtual bool performUndo(MapDocumentCommandFacade* document);

            bool isRepeatDelimiter() const;
            bool isRepeatable(MapDocumentCommandFacade* document) const;
            UndoableCommand* repeat(MapDocumentCommandFacade* document) const;
            
            virtual bool collateWith(UndoableCommand* command);
        private:
            virtual bool doPerformUndo(MapDocumentCommandFacade* document) = 0;
            
            virtual bool doIsRepeatDelimiter() const;
            virtual bool doIsRepeatable(MapDocumentCommandFacade* document) const = 0;
            virtual UndoableCommand* doRepeat(MapDocumentCommandFacade* document) const;
            
            virtual bool doCollateWith(UndoableCommand* command) = 0;
        public: // this method is just a service for DocumentCommand and should never be called from anywhere else
            virtual size_t documentModificationCount() const;
        };
    }
}

#endif /* defined(__TrenchBroom__UndoableCommand__) */
