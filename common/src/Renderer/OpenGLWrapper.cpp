/*
 Copyright (C) 2020 Marc Schraffenberger

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

#include "OpenGLWrapper.h"

#include <QOpenGLDebugLogger>

namespace TrenchBroom {
    namespace Renderer {
        OpenGLWrapper(GLContext& glContext) : 
        QOpenGLFunctions(&glContext),
        m_debugLogger(new QOpenGLDebugLogger(&glContext)) {}
        
        OpenGLWrapper::~OpenGLWrapper() {
            delete m_debugLogger;
        }

        void OpenGLWrapper::initialize() {            
            connect(*m_debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

            if (m_debugLogger->initialize()) {
                m_debugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
                m_debugLogger->enableMessages();
            }

            m_debugLogger->initialize();
            initializeOpenGLFunctions();
        }

        void OpenGLWrapper::onDebugLogMessage(QOpenGLDebugMessage message) {
            if (message.type() & ErrorType != 0) {
                throw RenderException("OpenGL error: " + std::to_string(message.id()) + " (" + message.message() + ") ");
            }
        }
    }
}
