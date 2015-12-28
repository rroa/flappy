#include "SDLWrapper.hpp"
#include "TimeManager.hpp"
//
// Constants
//
const float DESIRED_FRAME_RATE = 60.0f;
const float DESIRED_FRAME_TIME = 1.0f / DESIRED_FRAME_RATE;

namespace Flappy
{
    SDLWrapper::SDLWrapper( const std::string& title, const int& width, const int& height )
        : m_title       ( title )
        , m_width       ( width )
        , m_height      ( height )
        , m_nUpdates    ( 0 )
        , m_deltaTime   ( 0.0 )
    {
        // Initializing the main window pointer
        //
        m_mainwindow = 0;

        // Setting the Simulation state
        //
        m_state = SimulationState::RUNNING;

        // Initializing members
        //
        m_timer = new TimeManager;
    }

    SDLWrapper::~SDLWrapper( )
    {
        OnCleanup( );
    }

    bool SDLWrapper::OnInit( )
    {

        if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
        {
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // Setting GL attributes
        //
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,         1 );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,          16 );
        SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE,         32 );

        /*
         * There's something about multi-sampling that my VM doesn't like
         * SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS,  1 );
         * SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES,  2 );
         */

        m_mainwindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
             m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        /* Create our opengl context and attach it to our window */
        m_maincontext = SDL_GL_CreateContext( m_mainwindow );
        SDL_GL_MakeCurrent( m_mainwindow, m_maincontext );

         /* This makes our buffer swap syncronized with the monitor's vertical refresh */
        SDL_GL_SetSwapInterval( 1 );

        GLenum glew_status = glewInit();
        if (GLEW_OK != glew_status) {
            #if !EMSCRIPTEN
                std::cerr  << "Error: %s\n", glewGetErrorString( glew_status );
            #endif
            return false;
        }

        if (!GLEW_VERSION_2_0) {
            #if !EMSCRIPTEN
                std::cerr  << "No support for OpenGL 2.0 found\n";
            #endif
            return false;
        }
//
//         // GameRoot::getInstance()->onInitView();
//
        return true;
    }

    void SDLWrapper::OnCleanup( )
    {
        // SDL Cleanup
        //
        SDL_GL_DeleteContext( m_maincontext );
        SDL_DestroyWindow( m_mainwindow );
        SDL_Quit( );
    }

    void SDLWrapper::OnResize( int width, int height )
    {
        m_width = width;
        m_height = height;
    }

    void SDLWrapper::OnExit( )
    {
        m_state = SimulationState::QUIT;
    }

    void SDLWrapper::OnExecute( )
    {
        // Frame starts
        //
        double startTime = m_timer->GetElapsedTime( );

        // Input
        //
        OnInput( );

        // Update
        //
        OnUpdate( );

        // Draw
        //
        OnRender( );


        // Time Management
        //
        double endTime = m_timer->GetElapsedTime( );
        double nextTimeFrame = startTime + DESIRED_FRAME_TIME;

        while( endTime < nextTimeFrame )
        {
            // Spin lock
            //
            endTime = m_timer->GetElapsedTime( );
        }

        // Frame ends
        //
        ++m_nUpdates;
        m_deltaTime = m_nUpdates / static_cast< double >( DESIRED_FRAME_RATE );
    }

    void SDLWrapper::OnInput( )
    {
        SDL_Event event;

        if( SDL_PollEvent( &event ))
        {
            // Clean exit if window is closed
            //
            if( event.type == SDL_QUIT )
            {
                OnExit( );
            }

            switch( event.type )
            {
                case SDL_KEYDOWN:
                    OnKeyDown( event.key );
                    break;

                case SDL_KEYUP:
                    OnKeyUp( event.key );
                    break;

                case SDL_MOUSEMOTION:
                    OnMouseMove( event.motion.x,
                                 event.motion.y,
                                 event.motion.xrel,
                                 event.motion.yrel,
                                 ( event.motion.state & SDL_BUTTON( SDL_BUTTON_LEFT   )) != 0,   // Left button clicked
                                 ( event.motion.state & SDL_BUTTON( SDL_BUTTON_RIGHT  )) != 0,   // Right button clicked
                                 ( event.motion.state & SDL_BUTTON( SDL_BUTTON_MIDDLE )) != 0 ); // Middle button clicked
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    HandleMouseButtonDownEvents( &event );
                    break;

                case SDL_MOUSEBUTTONUP:
                    HandleMouseButtonUpEvents( &event );
                    break;

                #if EMSCRIPTEN
                case SDL_FINGERMOTION:
                    OnFingerMotion( &event );
                    break;

                case SDL_FINGERDOWN:
                    OnFingerDown( &event );
                    break;

                case SDL_FINGERUP:
                    OnFingerUp( &event );
                    break;
                #endif
            }
        }
    }

    void SDLWrapper::HandleMouseButtonDownEvents( SDL_Event* event )
    {
        switch( event->button.button )
        {
            case SDL_BUTTON_LEFT:
                OnLButtonDown( event->button.x, event->button.y );
                break;

            case SDL_BUTTON_RIGHT:
                OnRButtonDown( event->button.x, event->button.y );
                break;

            case SDL_BUTTON_MIDDLE:
                OnMButtonDown( event->button.x, event->button.y );
                break;
        }
    }

    void SDLWrapper::HandleMouseButtonUpEvents( SDL_Event* event )
    {
        switch( event->button.button )
        {
            case SDL_BUTTON_LEFT:
                OnLButtonUp( event->button.x, event->button.y );
                break;

            case SDL_BUTTON_RIGHT:
                OnRButtonUp( event->button.x, event->button.y );
                break;

            case SDL_BUTTON_MIDDLE:
                OnMButtonUp( event->button.x, event->button.y );
                break;
        }
    }

    void SDLWrapper::OnMouseMove( int x, int y, int relx, int rely, bool left, bool right, bool middle )
    {
        // std::cout << "Mouse Move: " << x << ", " << y << std::endl;
    }

    void SDLWrapper::OnLButtonDown( int x, int y)
    {
        // std::cout << "Left Button Mouse Down: " << x << ", " << y << std::endl;
    }

    void SDLWrapper::OnRButtonDown( int x, int y )
    {
        // std::cout << "Right Button Mouse Down: " << x << ", " << y << std::endl;
    }

    void SDLWrapper::OnMButtonDown( int x, int y )
    {
        // std::cout << "Middle Button Mouse Up: " << x << ", " << y << std::endl;
    }

    void SDLWrapper::OnLButtonUp( int x, int y )
    {
        // std::cout << "Left Button Mouse Up: " << x << ", " << y << std::endl;
    }

    void SDLWrapper::OnRButtonUp( int x, int y )
    {
        // std::cout << "Right Button Mouse Up: " << x << ", " << y << std::endl;
    }

    void SDLWrapper::OnMButtonUp( int x, int y )
    {
        // std::cout << "Middle Button Mouse Up: " << x << ", " << y << std::endl;
    }

    void SDLWrapper::OnFingerDown( SDL_Event* touchFingerEvent )
    {
        // TODO: Comeback to this
        // SDL_TouchFingerEvent* t = (SDL_TouchFingerEvent*)touchFingerEvent;
    }

    void SDLWrapper::OnFingerUp( SDL_Event* touchFingerEvent )
    {
        // TODO: Comeback to this
        // SDL_TouchFingerEvent* t = (SDL_TouchFingerEvent*)touchFingerEvent;
    }

    void SDLWrapper::OnFingerMotion( SDL_Event* touchFingerEvent )
    {
        // TODO: Comeback to thiss
        // SDL_TouchFingerEvent* t = (SDL_TouchFingerEvent*)touchFingerEvent;
    }

    void SDLWrapper::OnUpdate( )
    {

    }

    void SDLWrapper::OnKeyDown( SDL_KeyboardEvent keyBoardEvent )
    {
        // std::cout << "Key down: " << keyBoardEvent.keysym.sym << std::endl;
        // printf("Physical %s key acting as %s key\n", SDL_GetScancodeName(keyBoardEvent.keysym.scancode),
        //   SDL_GetKeyName(keyBoardEvent.keysym.sym));
    }

    void SDLWrapper::OnKeyUp( SDL_KeyboardEvent keyBoardEvent )
    {
        if( keyBoardEvent.keysym.sym == SDLK_ESCAPE )
        {
            OnExit( );
        }

        // std::cout << "Key up: " << keyBoardEvent.keysym.sym << std::endl;
        // printf("Physical %s key acting as %s key\n", SDL_GetScancodeName(keyBoardEvent.keysym.scancode),
        //   SDL_GetKeyName(keyBoardEvent.keysym.sym));
    }

    void SDLWrapper::OnRender( )
    {
        // Bringing the back buffer to the front
        //
        glClearColor( 0.1f, 0.1f, 0.15f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );
        SDL_GL_SwapWindow( m_mainwindow );
    }
}
