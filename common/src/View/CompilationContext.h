/*
 Copyright (C) 2010-2016 Kristian Duske
 
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

#ifndef CompilationContext_h
#define CompilationContext_h

#include "Logger.h"
#include "StringUtils.h"
#include "VariableHelper.h"
#include "View/ViewTypes.h"

#include <wx/string.h>
#include <wx/thread.h>

namespace TrenchBroom {
    namespace View {
        class CompilationContext {
        private:
            MapDocumentSPtr m_document;
            VariableTable m_variables;
            VariableValueTable m_variableValues;
            
            wxCriticalSection m_outputSection;
            wxString m_output;
        public:
            CompilationContext(MapDocumentSPtr document, const VariableTable& variables, const VariableValueTable& variableValues);
            
            MapDocumentSPtr document() const;
            
            String translateVariables(const String& input) const;
            void redefineVariable(const String& variableName, const String& value);
        };
    }
}


#endif /* CompilationContext_h */
