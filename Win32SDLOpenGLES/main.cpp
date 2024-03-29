#include<conio.h>
#include<SDL2/SDL.h>
#include<GLES3/gl3.h>
#include"SimpleRenderer.h"
#include<iostream>
#include"GLES2_Context.h"

typedef struct
{
	/* SDL init flags */
	char** argv;
	Uint32 flags;
	Uint32 verbose;

	/* Video info */
	const char* videodriver;
	int display;
	const char* window_title;
	const char* window_icon;
	Uint32 window_flags;
	int window_x;
	int window_y;
	int window_w;
	int window_h;
	int window_minW;
	int window_minH;
	int window_maxW;
	int window_maxH;
	int logical_w;
	int logical_h;
	float scale;
	int depth;
	int refresh_rate;
	int num_windows;
	SDL_Window** windows;

	/* Renderer info */
	const char* renderdriver;
	Uint32 render_flags;
	SDL_bool skip_renderer;
	SDL_Renderer** renderers;
	SDL_Texture** targets;

	/* Audio info */
	const char* audiodriver;
	SDL_AudioSpec audiospec;

	/* GL settings */
	int gl_red_size;
	int gl_green_size;
	int gl_blue_size;
	int gl_alpha_size;
	int gl_buffer_size;
	int gl_depth_size;
	int gl_stencil_size;
	int gl_double_buffer;
	int gl_accum_red_size;
	int gl_accum_green_size;
	int gl_accum_blue_size;
	int gl_accum_alpha_size;
	int gl_stereo;
	int gl_multisamplebuffers;
	int gl_multisamplesamples;
	int gl_retained_backing;
	int gl_accelerated;
	int gl_major_version;
	int gl_minor_version;
	int gl_debug;
	int gl_profile_mask;
} SDLTest_CommonState;

static SDLTest_CommonState* state;
static SDL_GLContext* context = NULL;
static int depth = 16;
static GLES2_Context ctx;


#if defined(__PSP__)
#define DEFAULT_WINDOW_WIDTH  480
#define DEFAULT_WINDOW_HEIGHT 272
#else
#define DEFAULT_WINDOW_WIDTH  640
#define DEFAULT_WINDOW_HEIGHT 480
#endif

#define VERBOSE_VIDEO   0x00000001
#define VERBOSE_MODES   0x00000002
#define VERBOSE_RENDER  0x00000004
#define VERBOSE_EVENT   0x00000008
#define VERBOSE_AUDIO   0x00000010



SDLTest_CommonState* CommonCreateState(char** argv, Uint32 flags)
{
	int i;
	SDLTest_CommonState* state;

	state = (SDLTest_CommonState*)SDL_calloc(1, sizeof(*state));
	if (!state) {
		SDL_OutOfMemory();
		return NULL;
	}

	/* Initialize some defaults */
	state->argv = argv;
	state->flags = flags;
	state->window_title = argv[0];
	state->window_flags = 0;
	state->window_x = SDL_WINDOWPOS_UNDEFINED;
	state->window_y = SDL_WINDOWPOS_UNDEFINED;
	state->window_w = DEFAULT_WINDOW_WIDTH;
	state->window_h = DEFAULT_WINDOW_HEIGHT;
	state->num_windows = 1;
	state->audiospec.freq = 22050;
	state->audiospec.format = AUDIO_S16;
	state->audiospec.channels = 2;
	state->audiospec.samples = 2048;

	/* Set some very sane GL defaults */
	state->gl_red_size = 3;
	state->gl_green_size = 3;
	state->gl_blue_size = 2;
	state->gl_alpha_size = 0;
	state->gl_buffer_size = 0;
	state->gl_depth_size = 16;
	state->gl_stencil_size = 0;
	state->gl_double_buffer = 1;
	state->gl_accum_red_size = 0;
	state->gl_accum_green_size = 0;
	state->gl_accum_blue_size = 0;
	state->gl_accum_alpha_size = 0;
	state->gl_stereo = 0;
	state->gl_multisamplebuffers = 0;
	state->gl_multisamplesamples = 0;
	state->gl_retained_backing = 1;
	state->gl_accelerated = -1;
	state->gl_debug = 0;

	return state;
}








static void SDL_snprintfcat(SDL_OUT_Z_CAP(maxlen) char* text, size_t maxlen, SDL_PRINTF_FORMAT_STRING const char* fmt, ...)
{
	size_t length = SDL_strlen(text);
	va_list ap;

	va_start(ap, fmt);
	text += length;
	maxlen -= length;
	SDL_vsnprintf(text, maxlen, fmt, ap);
	va_end(ap);
}

static void
SDLTest_PrintRendererFlag(char* text, size_t maxlen, Uint32 flag)
{
	switch (flag) {
	case SDL_RENDERER_SOFTWARE:
		SDL_snprintfcat(text, maxlen, "Software");
		break;
	case SDL_RENDERER_ACCELERATED:
		SDL_snprintfcat(text, maxlen, "Accelerated");
		break;
	case SDL_RENDERER_PRESENTVSYNC:
		SDL_snprintfcat(text, maxlen, "PresentVSync");
		break;
	case SDL_RENDERER_TARGETTEXTURE:
		SDL_snprintfcat(text, maxlen, "TargetTexturesSupported");
		break;
	default:
		SDL_snprintfcat(text, maxlen, "0x%8.8x", flag);
		break;
	}
}

static void
SDLTest_PrintPixelFormat(char* text, size_t maxlen, Uint32 format)
{
	switch (format) {
	case SDL_PIXELFORMAT_UNKNOWN:
		SDL_snprintfcat(text, maxlen, "Unknown");
		break;
	case SDL_PIXELFORMAT_INDEX1LSB:
		SDL_snprintfcat(text, maxlen, "Index1LSB");
		break;
	case SDL_PIXELFORMAT_INDEX1MSB:
		SDL_snprintfcat(text, maxlen, "Index1MSB");
		break;
	case SDL_PIXELFORMAT_INDEX4LSB:
		SDL_snprintfcat(text, maxlen, "Index4LSB");
		break;
	case SDL_PIXELFORMAT_INDEX4MSB:
		SDL_snprintfcat(text, maxlen, "Index4MSB");
		break;
	case SDL_PIXELFORMAT_INDEX8:
		SDL_snprintfcat(text, maxlen, "Index8");
		break;
	case SDL_PIXELFORMAT_RGB332:
		SDL_snprintfcat(text, maxlen, "RGB332");
		break;
	case SDL_PIXELFORMAT_RGB444:
		SDL_snprintfcat(text, maxlen, "RGB444");
		break;
	case SDL_PIXELFORMAT_RGB555:
		SDL_snprintfcat(text, maxlen, "RGB555");
		break;
	case SDL_PIXELFORMAT_BGR555:
		SDL_snprintfcat(text, maxlen, "BGR555");
		break;
	case SDL_PIXELFORMAT_ARGB4444:
		SDL_snprintfcat(text, maxlen, "ARGB4444");
		break;
	case SDL_PIXELFORMAT_ABGR4444:
		SDL_snprintfcat(text, maxlen, "ABGR4444");
		break;
	case SDL_PIXELFORMAT_ARGB1555:
		SDL_snprintfcat(text, maxlen, "ARGB1555");
		break;
	case SDL_PIXELFORMAT_ABGR1555:
		SDL_snprintfcat(text, maxlen, "ABGR1555");
		break;
	case SDL_PIXELFORMAT_RGB565:
		SDL_snprintfcat(text, maxlen, "RGB565");
		break;
	case SDL_PIXELFORMAT_BGR565:
		SDL_snprintfcat(text, maxlen, "BGR565");
		break;
	case SDL_PIXELFORMAT_RGB24:
		SDL_snprintfcat(text, maxlen, "RGB24");
		break;
	case SDL_PIXELFORMAT_BGR24:
		SDL_snprintfcat(text, maxlen, "BGR24");
		break;
	case SDL_PIXELFORMAT_RGB888:
		SDL_snprintfcat(text, maxlen, "RGB888");
		break;
	case SDL_PIXELFORMAT_BGR888:
		SDL_snprintfcat(text, maxlen, "BGR888");
		break;
	case SDL_PIXELFORMAT_ARGB8888:
		SDL_snprintfcat(text, maxlen, "ARGB8888");
		break;
	case SDL_PIXELFORMAT_RGBA8888:
		SDL_snprintfcat(text, maxlen, "RGBA8888");
		break;
	case SDL_PIXELFORMAT_ABGR8888:
		SDL_snprintfcat(text, maxlen, "ABGR8888");
		break;
	case SDL_PIXELFORMAT_BGRA8888:
		SDL_snprintfcat(text, maxlen, "BGRA8888");
		break;
	case SDL_PIXELFORMAT_ARGB2101010:
		SDL_snprintfcat(text, maxlen, "ARGB2101010");
		break;
	case SDL_PIXELFORMAT_YV12:
		SDL_snprintfcat(text, maxlen, "YV12");
		break;
	case SDL_PIXELFORMAT_IYUV:
		SDL_snprintfcat(text, maxlen, "IYUV");
		break;
	case SDL_PIXELFORMAT_YUY2:
		SDL_snprintfcat(text, maxlen, "YUY2");
		break;
	case SDL_PIXELFORMAT_UYVY:
		SDL_snprintfcat(text, maxlen, "UYVY");
		break;
	case SDL_PIXELFORMAT_YVYU:
		SDL_snprintfcat(text, maxlen, "YVYU");
		break;
	case SDL_PIXELFORMAT_NV12:
		SDL_snprintfcat(text, maxlen, "NV12");
		break;
	case SDL_PIXELFORMAT_NV21:
		SDL_snprintfcat(text, maxlen, "NV21");
		break;
	default:
		SDL_snprintfcat(text, maxlen, "0x%8.8x", format);
		break;
	}
}

static void
SDLTest_PrintRenderer(SDL_RendererInfo* info)
{
	int i, count;
	char text[1024];

	SDL_Log("  Renderer %s:\n", info->name);

	SDL_snprintf(text, sizeof(text), "    Flags: 0x%8.8X", info->flags);
	SDL_snprintfcat(text, sizeof(text), " (");
	count = 0;
	for (i = 0; i < sizeof(info->flags) * 8; ++i) {
		Uint32 flag = (1 << i);
		if (info->flags & flag) {
			if (count > 0) {
				SDL_snprintfcat(text, sizeof(text), " | ");
			}
			SDLTest_PrintRendererFlag(text, sizeof(text), flag);
			++count;
		}
	}
	SDL_snprintfcat(text, sizeof(text), ")");
	SDL_Log("%s\n", text);

	SDL_snprintf(text, sizeof(text), "    Texture formats (%d): ", info->num_texture_formats);
	for (i = 0; i < (int)info->num_texture_formats; ++i) {
		if (i > 0) {
			SDL_snprintfcat(text, sizeof(text), ", ");
		}
		SDLTest_PrintPixelFormat(text, sizeof(text), info->texture_formats[i]);
	}
	SDL_Log("%s\n", text);

	if (info->max_texture_width || info->max_texture_height) {
		SDL_Log("    Max Texture Size: %dx%d\n",
			info->max_texture_width, info->max_texture_height);
	}
}

static SDL_Surface*
SDLTest_LoadIcon(const char* file)
{
	SDL_Surface* icon;

	/* Load the icon surface */
	icon = SDL_LoadBMP(file);
	if (icon == NULL) {
		SDL_Log("Couldn't load %s: %s\n", file, SDL_GetError());
		return (NULL);
	}

	if (icon->format->palette) {
		/* Set the colorkey */
		SDL_SetColorKey(icon, 1, *((Uint8*)icon->pixels));
	}

	return (icon);
}

static SDL_HitTestResult SDLCALL
SDLTest_ExampleHitTestCallback(SDL_Window* win, const SDL_Point* area, void* data)
{
	int w, h;
	const int RESIZE_BORDER = 8;
	const int DRAGGABLE_TITLE = 32;

	/*SDL_Log("Hit test point %d,%d\n", area->x, area->y);*/

	SDL_GetWindowSize(win, &w, &h);

	if (area->x < RESIZE_BORDER) {
		if (area->y < RESIZE_BORDER) {
			SDL_Log("SDL_HITTEST_RESIZE_TOPLEFT\n");
			return SDL_HITTEST_RESIZE_TOPLEFT;
		}
		else if (area->y >= (h - RESIZE_BORDER)) {
			SDL_Log("SDL_HITTEST_RESIZE_BOTTOMLEFT\n");
			return SDL_HITTEST_RESIZE_BOTTOMLEFT;
		}
		else {
			SDL_Log("SDL_HITTEST_RESIZE_LEFT\n");
			return SDL_HITTEST_RESIZE_LEFT;
		}
	}
	else if (area->x >= (w - RESIZE_BORDER)) {
		if (area->y < RESIZE_BORDER) {
			SDL_Log("SDL_HITTEST_RESIZE_TOPRIGHT\n");
			return SDL_HITTEST_RESIZE_TOPRIGHT;
		}
		else if (area->y >= (h - RESIZE_BORDER)) {
			SDL_Log("SDL_HITTEST_RESIZE_BOTTOMRIGHT\n");
			return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
		}
		else {
			SDL_Log("SDL_HITTEST_RESIZE_RIGHT\n");
			return SDL_HITTEST_RESIZE_RIGHT;
		}
	}
	else if (area->y >= (h - RESIZE_BORDER)) {
		SDL_Log("SDL_HITTEST_RESIZE_BOTTOM\n");
		return SDL_HITTEST_RESIZE_BOTTOM;
	}
	else if (area->y < RESIZE_BORDER) {
		SDL_Log("SDL_HITTEST_RESIZE_TOP\n");
		return SDL_HITTEST_RESIZE_TOP;
	}
	else if (area->y < DRAGGABLE_TITLE) {
		SDL_Log("SDL_HITTEST_DRAGGABLE\n");
		return SDL_HITTEST_DRAGGABLE;
	}
	return SDL_HITTEST_NORMAL;
}

SDL_bool CommonInit(SDLTest_CommonState* state)
{
	int i, j, m, n, w, h;
	SDL_DisplayMode fullscreen_mode;
	char text[1024];

	if (state->flags & SDL_INIT_VIDEO) {
		if (state->verbose & VERBOSE_VIDEO) {
			n = SDL_GetNumVideoDrivers();
			if (n == 0) {
				SDL_Log("No built-in video drivers\n");
			}
			else {
				SDL_snprintf(text, sizeof(text), "Built-in video drivers:");
				for (i = 0; i < n; ++i) {
					if (i > 0) {
						SDL_snprintfcat(text, sizeof(text), ",");
					}
					SDL_snprintfcat(text, sizeof(text), " %s", SDL_GetVideoDriver(i));
				}
				SDL_Log("%s\n", text);
			}
		}
		if (SDL_VideoInit(state->videodriver) < 0) {
			SDL_Log("Couldn't initialize video driver: %s\n",
				SDL_GetError());
			return SDL_FALSE;
		}
		if (state->verbose & VERBOSE_VIDEO) {
			SDL_Log("Video driver: %s\n",
				SDL_GetCurrentVideoDriver());
		}

		/* Upload GL settings */
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, state->gl_red_size);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, state->gl_green_size);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, state->gl_blue_size);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, state->gl_alpha_size);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, state->gl_double_buffer);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, state->gl_buffer_size);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, state->gl_depth_size);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, state->gl_stencil_size);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, state->gl_accum_red_size);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, state->gl_accum_green_size);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, state->gl_accum_blue_size);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, state->gl_accum_alpha_size);
		SDL_GL_SetAttribute(SDL_GL_STEREO, state->gl_stereo);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, state->gl_multisamplebuffers);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, state->gl_multisamplesamples);
		if (state->gl_accelerated >= 0) {
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,
				state->gl_accelerated);
		}
		SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, state->gl_retained_backing);
		if (state->gl_major_version) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, state->gl_major_version);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, state->gl_minor_version);
		}
		if (state->gl_debug) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		}
		if (state->gl_profile_mask) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, state->gl_profile_mask);
		}

		if (state->verbose & VERBOSE_MODES) {
			SDL_Rect bounds, usablebounds;
			float hdpi = 0;
			float vdpi = 0;
			SDL_DisplayMode mode;
			int bpp;
			Uint32 Rmask, Gmask, Bmask, Amask;
#if SDL_VIDEO_DRIVER_WINDOWS
			int adapterIndex = 0;
			int outputIndex = 0;
#endif
			n = SDL_GetNumVideoDisplays();
			SDL_Log("Number of displays: %d\n", n);
			for (i = 0; i < n; ++i) {
				SDL_Log("Display %d: %s\n", i, SDL_GetDisplayName(i));

				SDL_zero(bounds);
				SDL_GetDisplayBounds(i, &bounds);

				SDL_zero(usablebounds);
				SDL_GetDisplayUsableBounds(i, &usablebounds);

				SDL_GetDisplayDPI(i, NULL, &hdpi, &vdpi);

				SDL_Log("Bounds: %dx%d at %d,%d\n", bounds.w, bounds.h, bounds.x, bounds.y);
				SDL_Log("Usable bounds: %dx%d at %d,%d\n", usablebounds.w, usablebounds.h, usablebounds.x, usablebounds.y);
				SDL_Log("DPI: %fx%f\n", hdpi, vdpi);

				SDL_GetDesktopDisplayMode(i, &mode);
				SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask, &Gmask,
					&Bmask, &Amask);
				SDL_Log("  Current mode: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
					mode.w, mode.h, mode.refresh_rate, bpp,
					SDL_GetPixelFormatName(mode.format));
				if (Rmask || Gmask || Bmask) {
					SDL_Log("      Red Mask   = 0x%.8x\n", Rmask);
					SDL_Log("      Green Mask = 0x%.8x\n", Gmask);
					SDL_Log("      Blue Mask  = 0x%.8x\n", Bmask);
					if (Amask)
						SDL_Log("      Alpha Mask = 0x%.8x\n", Amask);
				}

				/* Print available fullscreen video modes */
				m = SDL_GetNumDisplayModes(i);
				if (m == 0) {
					SDL_Log("No available fullscreen video modes\n");
				}
				else {
					SDL_Log("  Fullscreen video modes:\n");
					for (j = 0; j < m; ++j) {
						SDL_GetDisplayMode(i, j, &mode);
						SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask,
							&Gmask, &Bmask, &Amask);
						SDL_Log("    Mode %d: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
							j, mode.w, mode.h, mode.refresh_rate, bpp,
							SDL_GetPixelFormatName(mode.format));
						if (Rmask || Gmask || Bmask) {
							SDL_Log("        Red Mask   = 0x%.8x\n",
								Rmask);
							SDL_Log("        Green Mask = 0x%.8x\n",
								Gmask);
							SDL_Log("        Blue Mask  = 0x%.8x\n",
								Bmask);
							if (Amask)
								SDL_Log("        Alpha Mask = 0x%.8x\n",
									Amask);
						}
					}
				}

#if SDL_VIDEO_DRIVER_WINDOWS
				/* Print the D3D9 adapter index */
				adapterIndex = SDL_Direct3D9GetAdapterIndex(i);
				SDL_Log("D3D9 Adapter Index: %d", adapterIndex);

				/* Print the DXGI adapter and output indices */
				SDL_DXGIGetOutputInfo(i, &adapterIndex, &outputIndex);
				SDL_Log("DXGI Adapter Index: %d  Output Index: %d", adapterIndex, outputIndex);
#endif
			}
		}

		if (state->verbose & VERBOSE_RENDER) {
			SDL_RendererInfo info;

			n = SDL_GetNumRenderDrivers();
			if (n == 0) {
				SDL_Log("No built-in render drivers\n");
			}
			else {
				SDL_Log("Built-in render drivers:\n");
				for (i = 0; i < n; ++i) {
					SDL_GetRenderDriverInfo(i, &info);
					SDLTest_PrintRenderer(&info);
				}
			}
		}

		SDL_zero(fullscreen_mode);
		switch (state->depth) {
		case 8:
			fullscreen_mode.format = SDL_PIXELFORMAT_INDEX8;
			break;
		case 15:
			fullscreen_mode.format = SDL_PIXELFORMAT_RGB555;
			break;
		case 16:
			fullscreen_mode.format = SDL_PIXELFORMAT_RGB565;
			break;
		case 24:
			fullscreen_mode.format = SDL_PIXELFORMAT_RGB24;
			break;
		default:
			fullscreen_mode.format = SDL_PIXELFORMAT_RGB888;
			break;
		}
		fullscreen_mode.refresh_rate = state->refresh_rate;

		state->windows =
			(SDL_Window**)SDL_calloc(state->num_windows,
				sizeof(*state->windows));
		state->renderers =
			(SDL_Renderer**)SDL_calloc(state->num_windows,
				sizeof(*state->renderers));
		state->targets =
			(SDL_Texture**)SDL_calloc(state->num_windows,
				sizeof(*state->targets));
		if (!state->windows || !state->renderers) {
			SDL_Log("Out of memory!\n");
			return SDL_FALSE;
		}
		for (i = 0; i < state->num_windows; ++i) {
			char title[1024];

			if (state->num_windows > 1) {
				SDL_snprintf(title, SDL_arraysize(title), "%s %d",
					state->window_title, i + 1);
			}
			else {
				SDL_strlcpy(title, state->window_title, SDL_arraysize(title));
			}
			state->windows[i] =
				SDL_CreateWindow(title, state->window_x, state->window_y,
					state->window_w, state->window_h,
					state->window_flags);
			if (!state->windows[i]) {
				SDL_Log("Couldn't create window: %s\n",
					SDL_GetError());
				return SDL_FALSE;
			}
			if (state->window_minW || state->window_minH) {
				SDL_SetWindowMinimumSize(state->windows[i], state->window_minW, state->window_minH);
			}
			if (state->window_maxW || state->window_maxH) {
				SDL_SetWindowMaximumSize(state->windows[i], state->window_maxW, state->window_maxH);
			}
			SDL_GetWindowSize(state->windows[i], &w, &h);
			if (!(state->window_flags & SDL_WINDOW_RESIZABLE) &&
				(w != state->window_w || h != state->window_h)) {
				printf("Window requested size %dx%d, got %dx%d\n", state->window_w, state->window_h, w, h);
				state->window_w = w;
				state->window_h = h;
			}
			if (SDL_SetWindowDisplayMode(state->windows[i], &fullscreen_mode) < 0) {
				SDL_Log("Can't set up fullscreen display mode: %s\n",
					SDL_GetError());
				return SDL_FALSE;
			}

			/* Add resize/drag areas for windows that are borderless and resizable */
			if ((state->window_flags & (SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS)) ==
				(SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS)) {
				SDL_SetWindowHitTest(state->windows[i], SDLTest_ExampleHitTestCallback, NULL);
			}

			if (state->window_icon) {
				SDL_Surface* icon = SDLTest_LoadIcon(state->window_icon);
				if (icon) {
					SDL_SetWindowIcon(state->windows[i], icon);
					SDL_FreeSurface(icon);
				}
			}

			SDL_ShowWindow(state->windows[i]);

			if (!state->skip_renderer
				&& (state->renderdriver
					|| !(state->window_flags & (SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN)))) {
				m = -1;
				if (state->renderdriver) {
					SDL_RendererInfo info;
					n = SDL_GetNumRenderDrivers();
					for (j = 0; j < n; ++j) {
						SDL_GetRenderDriverInfo(j, &info);
						if (SDL_strcasecmp(info.name, state->renderdriver) ==
							0) {
							m = j;
							break;
						}
					}
					if (m == -1) {
						SDL_Log("Couldn't find render driver named %s",
							state->renderdriver);
						return SDL_FALSE;
					}
				}
				state->renderers[i] = SDL_CreateRenderer(state->windows[i],
					m, state->render_flags);
				if (!state->renderers[i]) {
					SDL_Log("Couldn't create renderer: %s\n",
						SDL_GetError());
					return SDL_FALSE;
				}
				if (state->logical_w && state->logical_h) {
					SDL_RenderSetLogicalSize(state->renderers[i], state->logical_w, state->logical_h);
				}
				else if (state->scale != 0.) {
					SDL_RenderSetScale(state->renderers[i], state->scale, state->scale);
				}
				if (state->verbose & VERBOSE_RENDER) {
					SDL_RendererInfo info;

					SDL_Log("Current renderer:\n");
					SDL_GetRendererInfo(state->renderers[i], &info);
					SDLTest_PrintRenderer(&info);
				}
			}
		}
	}

	if (state->flags & SDL_INIT_AUDIO) {
		if (state->verbose & VERBOSE_AUDIO) {
			n = SDL_GetNumAudioDrivers();
			if (n == 0) {
				SDL_Log("No built-in audio drivers\n");
			}
			else {
				SDL_snprintf(text, sizeof(text), "Built-in audio drivers:");
				for (i = 0; i < n; ++i) {
					if (i > 0) {
						SDL_snprintfcat(text, sizeof(text), ",");
					}
					SDL_snprintfcat(text, sizeof(text), " %s", SDL_GetAudioDriver(i));
				}
				SDL_Log("%s\n", text);
			}
		}
		if (SDL_AudioInit(state->audiodriver) < 0) {
			SDL_Log("Couldn't initialize audio driver: %s\n",
				SDL_GetError());
			return SDL_FALSE;
		}
		if (state->verbose & VERBOSE_VIDEO) {
			SDL_Log("Audio driver: %s\n",
				SDL_GetCurrentAudioDriver());
		}

		if (SDL_OpenAudio(&state->audiospec, NULL) < 0) {
			SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
			return SDL_FALSE;
		}
	}

	return SDL_TRUE;
}





void
SDLTest_CommonQuit(SDLTest_CommonState* state)
{
	int i;

	SDL_free(state->windows);
	if (state->targets) {
		for (i = 0; i < state->num_windows; ++i) {
			if (state->targets[i]) {
				SDL_DestroyTexture(state->targets[i]);
			}
		}
		SDL_free(state->targets);
	}
	if (state->renderers) {
		for (i = 0; i < state->num_windows; ++i) {
			if (state->renderers[i]) {
				SDL_DestroyRenderer(state->renderers[i]);
			}
		}
		SDL_free(state->renderers);
	}
	if (state->flags & SDL_INIT_VIDEO) {
		SDL_VideoQuit();
	}
	if (state->flags & SDL_INIT_AUDIO) {
		SDL_AudioQuit();
	}
	SDL_free(state);
	SDL_Quit();
	//SDLTest_LogAllocations();
}
static void
quit(int rc)
{
	int i;

	if (context != NULL) {
		for (i = 0; i < state->num_windows; i++) {
			if (context[i]) {
				SDL_GL_DeleteContext(context[i]);
			}
		}

		SDL_free(context);
	}

	SDLTest_CommonQuit(state);
	_getch();
	exit(rc);
}

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = ctx.glGetError(); \
          if(glError != GL_NO_ERROR) { \
            SDL_Log("glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
            quit(1); \
          } \
        }

static const char*
DisplayOrientationName(int orientation)
{
	switch (orientation)
	{
#define CASE(X) case SDL_ORIENTATION_##X: return #X
		CASE(UNKNOWN);
		CASE(LANDSCAPE);
		CASE(LANDSCAPE_FLIPPED);
		CASE(PORTRAIT);
		CASE(PORTRAIT_FLIPPED);
#undef CASE
	default: return "???";
	}
}
static const char*
ControllerAxisName(const SDL_GameControllerAxis axis)
{
	switch (axis)
	{
#define AXIS_CASE(ax) case SDL_CONTROLLER_AXIS_##ax: return #ax
		AXIS_CASE(INVALID);
		AXIS_CASE(LEFTX);
		AXIS_CASE(LEFTY);
		AXIS_CASE(RIGHTX);
		AXIS_CASE(RIGHTY);
		AXIS_CASE(TRIGGERLEFT);
		AXIS_CASE(TRIGGERRIGHT);
#undef AXIS_CASE
	default: return "???";
	}
}

static const char*
ControllerButtonName(const SDL_GameControllerButton button)
{
	switch (button)
	{
#define BUTTON_CASE(btn) case SDL_CONTROLLER_BUTTON_##btn: return #btn
		BUTTON_CASE(INVALID);
		BUTTON_CASE(A);
		BUTTON_CASE(B);
		BUTTON_CASE(X);
		BUTTON_CASE(Y);
		BUTTON_CASE(BACK);
		BUTTON_CASE(GUIDE);
		BUTTON_CASE(START);
		BUTTON_CASE(LEFTSTICK);
		BUTTON_CASE(RIGHTSTICK);
		BUTTON_CASE(LEFTSHOULDER);
		BUTTON_CASE(RIGHTSHOULDER);
		BUTTON_CASE(DPAD_UP);
		BUTTON_CASE(DPAD_DOWN);
		BUTTON_CASE(DPAD_LEFT);
		BUTTON_CASE(DPAD_RIGHT);
#undef BUTTON_CASE
	default: return "???";
	}
}

static void
SDLTest_PrintEvent(SDL_Event* event)
{
	if ((event->type == SDL_MOUSEMOTION) || (event->type == SDL_FINGERMOTION)) {
		/* Mouse and finger motion are really spammy */
		return;
	}

	switch (event->type) {
	case SDL_DISPLAYEVENT:
		switch (event->display.event) {
		case SDL_DISPLAYEVENT_ORIENTATION:
			SDL_Log("SDL EVENT: Display %d changed orientation to %s", event->display.display, DisplayOrientationName(event->display.data1));
			break;
		default:
			SDL_Log("SDL EVENT: Display %d got unknown event 0x%4.4x",
				event->display.display, event->display.event);
			break;
		}
		break;
	case SDL_WINDOWEVENT:
		switch (event->window.event) {
		case SDL_WINDOWEVENT_SHOWN:
			SDL_Log("SDL EVENT: Window %d shown", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_HIDDEN:
			SDL_Log("SDL EVENT: Window %d hidden", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_Log("SDL EVENT: Window %d exposed", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_MOVED:
			SDL_Log("SDL EVENT: Window %d moved to %d,%d",
				event->window.windowID, event->window.data1,
				event->window.data2);
			break;
		case SDL_WINDOWEVENT_RESIZED:
			SDL_Log("SDL EVENT: Window %d resized to %dx%d",
				event->window.windowID, event->window.data1,
				event->window.data2);
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			SDL_Log("SDL EVENT: Window %d changed size to %dx%d",
				event->window.windowID, event->window.data1,
				event->window.data2);
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			SDL_Log("SDL EVENT: Window %d minimized", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			SDL_Log("SDL EVENT: Window %d maximized", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_RESTORED:
			SDL_Log("SDL EVENT: Window %d restored", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_ENTER:
			SDL_Log("SDL EVENT: Mouse entered window %d",
				event->window.windowID);
			break;
		case SDL_WINDOWEVENT_LEAVE:
			SDL_Log("SDL EVENT: Mouse left window %d", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			SDL_Log("SDL EVENT: Window %d gained keyboard focus",
				event->window.windowID);
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			SDL_Log("SDL EVENT: Window %d lost keyboard focus",
				event->window.windowID);
			break;
		case SDL_WINDOWEVENT_CLOSE:
			SDL_Log("SDL EVENT: Window %d closed", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_TAKE_FOCUS:
			SDL_Log("SDL EVENT: Window %d take focus", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_HIT_TEST:
			SDL_Log("SDL EVENT: Window %d hit test", event->window.windowID);
			break;
		default:
			SDL_Log("SDL EVENT: Window %d got unknown event 0x%4.4x",
				event->window.windowID, event->window.event);
			break;
		}
		break;
	case SDL_KEYDOWN:
		SDL_Log("SDL EVENT: Keyboard: key pressed  in window %d: scancode 0x%08X = %s, keycode 0x%08X = %s",
			event->key.windowID,
			event->key.keysym.scancode,
			SDL_GetScancodeName(event->key.keysym.scancode),
			event->key.keysym.sym, SDL_GetKeyName(event->key.keysym.sym));
		break;
	case SDL_KEYUP:
		SDL_Log("SDL EVENT: Keyboard: key released in window %d: scancode 0x%08X = %s, keycode 0x%08X = %s",
			event->key.windowID,
			event->key.keysym.scancode,
			SDL_GetScancodeName(event->key.keysym.scancode),
			event->key.keysym.sym, SDL_GetKeyName(event->key.keysym.sym));
		break;
	case SDL_TEXTEDITING:
		SDL_Log("SDL EVENT: Keyboard: text editing \"%s\" in window %d",
			event->edit.text, event->edit.windowID);
		break;
	case SDL_TEXTINPUT:
		SDL_Log("SDL EVENT: Keyboard: text input \"%s\" in window %d",
			event->text.text, event->text.windowID);
		break;
	case SDL_KEYMAPCHANGED:
		SDL_Log("SDL EVENT: Keymap changed");
		break;
	case SDL_MOUSEMOTION:
		SDL_Log("SDL EVENT: Mouse: moved to %d,%d (%d,%d) in window %d",
			event->motion.x, event->motion.y,
			event->motion.xrel, event->motion.yrel,
			event->motion.windowID);
		break;
	case SDL_MOUSEBUTTONDOWN:
		SDL_Log("SDL EVENT: Mouse: button %d pressed at %d,%d with click count %d in window %d",
			event->button.button, event->button.x, event->button.y, event->button.clicks,
			event->button.windowID);
		break;
	case SDL_MOUSEBUTTONUP:
		SDL_Log("SDL EVENT: Mouse: button %d released at %d,%d with click count %d in window %d",
			event->button.button, event->button.x, event->button.y, event->button.clicks,
			event->button.windowID);
		break;
	case SDL_MOUSEWHEEL:
		SDL_Log("SDL EVENT: Mouse: wheel scrolled %d in x and %d in y (reversed: %d) in window %d",
			event->wheel.x, event->wheel.y, event->wheel.direction, event->wheel.windowID);
		break;
	case SDL_JOYDEVICEADDED:
		SDL_Log("SDL EVENT: Joystick index %d attached",
			event->jdevice.which);
		break;
	case SDL_JOYDEVICEREMOVED:
		SDL_Log("SDL EVENT: Joystick %d removed",
			event->jdevice.which);
		break;
	case SDL_JOYBALLMOTION:
		SDL_Log("SDL EVENT: Joystick %d: ball %d moved by %d,%d",
			event->jball.which, event->jball.ball, event->jball.xrel,
			event->jball.yrel);
		break;
	case SDL_JOYHATMOTION:
	{
		const char* position = "UNKNOWN";
		switch (event->jhat.value) {
		case SDL_HAT_CENTERED:
			position = "CENTER";
			break;
		case SDL_HAT_UP:
			position = "UP";
			break;
		case SDL_HAT_RIGHTUP:
			position = "RIGHTUP";
			break;
		case SDL_HAT_RIGHT:
			position = "RIGHT";
			break;
		case SDL_HAT_RIGHTDOWN:
			position = "RIGHTDOWN";
			break;
		case SDL_HAT_DOWN:
			position = "DOWN";
			break;
		case SDL_HAT_LEFTDOWN:
			position = "LEFTDOWN";
			break;
		case SDL_HAT_LEFT:
			position = "LEFT";
			break;
		case SDL_HAT_LEFTUP:
			position = "LEFTUP";
			break;
		}
		SDL_Log("SDL EVENT: Joystick %d: hat %d moved to %s", event->jhat.which,
			event->jhat.hat, position);
	}
	break;
	case SDL_JOYBUTTONDOWN:
		SDL_Log("SDL EVENT: Joystick %d: button %d pressed",
			event->jbutton.which, event->jbutton.button);
		break;
	case SDL_JOYBUTTONUP:
		SDL_Log("SDL EVENT: Joystick %d: button %d released",
			event->jbutton.which, event->jbutton.button);
		break;
	case SDL_CONTROLLERDEVICEADDED:
		SDL_Log("SDL EVENT: Controller index %d attached",
			event->cdevice.which);
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		SDL_Log("SDL EVENT: Controller %d removed",
			event->cdevice.which);
		break;
	case SDL_CONTROLLERAXISMOTION:
		SDL_Log("SDL EVENT: Controller %d axis %d ('%s') value: %d",
			event->caxis.which,
			event->caxis.axis,
			ControllerAxisName((SDL_GameControllerAxis)event->caxis.axis),
			event->caxis.value);
		break;
	case SDL_CONTROLLERBUTTONDOWN:
		SDL_Log("SDL EVENT: Controller %d button %d ('%s') down",
			event->cbutton.which, event->cbutton.button,
			ControllerButtonName((SDL_GameControllerButton)event->cbutton.button));
		break;
	case SDL_CONTROLLERBUTTONUP:
		SDL_Log("SDL EVENT: Controller %d button %d ('%s') up",
			event->cbutton.which, event->cbutton.button,
			ControllerButtonName((SDL_GameControllerButton)event->cbutton.button));
		break;
	case SDL_CLIPBOARDUPDATE:
		SDL_Log("SDL EVENT: Clipboard updated");
		break;

	case SDL_FINGERMOTION:
		SDL_Log("SDL EVENT: Finger: motion touch=%ld, finger=%ld, x=%f, y=%f, dx=%f, dy=%f, pressure=%f",
			(long)event->tfinger.touchId,
			(long)event->tfinger.fingerId,
			event->tfinger.x, event->tfinger.y,
			event->tfinger.dx, event->tfinger.dy, event->tfinger.pressure);
		break;
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
		SDL_Log("SDL EVENT: Finger: %s touch=%ld, finger=%ld, x=%f, y=%f, dx=%f, dy=%f, pressure=%f",
			(event->type == SDL_FINGERDOWN) ? "down" : "up",
			(long)event->tfinger.touchId,
			(long)event->tfinger.fingerId,
			event->tfinger.x, event->tfinger.y,
			event->tfinger.dx, event->tfinger.dy, event->tfinger.pressure);
		break;
	case SDL_DOLLARGESTURE:
		SDL_Log("SDL_EVENT: Dollar gesture detect: %ld", (long)event->dgesture.gestureId);
		break;
	case SDL_DOLLARRECORD:
		SDL_Log("SDL_EVENT: Dollar gesture record: %ld", (long)event->dgesture.gestureId);
		break;
	case SDL_MULTIGESTURE:
		SDL_Log("SDL_EVENT: Multi gesture fingers: %d", event->mgesture.numFingers);
		break;

	case SDL_RENDER_DEVICE_RESET:
		SDL_Log("SDL EVENT: render device reset");
		break;
	case SDL_RENDER_TARGETS_RESET:
		SDL_Log("SDL EVENT: render targets reset");
		break;

	case SDL_APP_TERMINATING:
		SDL_Log("SDL EVENT: App terminating");
		break;
	case SDL_APP_LOWMEMORY:
		SDL_Log("SDL EVENT: App running low on memory");
		break;
	case SDL_APP_WILLENTERBACKGROUND:
		SDL_Log("SDL EVENT: App will enter the background");
		break;
	case SDL_APP_DIDENTERBACKGROUND:
		SDL_Log("SDL EVENT: App entered the background");
		break;
	case SDL_APP_WILLENTERFOREGROUND:
		SDL_Log("SDL EVENT: App will enter the foreground");
		break;
	case SDL_APP_DIDENTERFOREGROUND:
		SDL_Log("SDL EVENT: App entered the foreground");
		break;
	case SDL_DROPBEGIN:
		SDL_Log("SDL EVENT: Drag and drop beginning");
		break;
	case SDL_DROPFILE:
		SDL_Log("SDL EVENT: Drag and drop file: '%s'", event->drop.file);
		break;
	case SDL_DROPTEXT:
		SDL_Log("SDL EVENT: Drag and drop text: '%s'", event->drop.file);
		break;
	case SDL_DROPCOMPLETE:
		SDL_Log("SDL EVENT: Drag and drop ending");
		break;
	case SDL_QUIT:
		SDL_Log("SDL EVENT: Quit requested");
		break;
	case SDL_USEREVENT:
		SDL_Log("SDL EVENT: User event %d", event->user.code);
		break;
	default:
		SDL_Log("Unknown event 0x%4.4x", event->type);
		break;
	}
}

static void
SDLTest_ScreenShot(SDL_Renderer* renderer)
{
	SDL_Rect viewport;
	SDL_Surface* surface;

	if (!renderer) {
		return;
	}

	SDL_RenderGetViewport(renderer, &viewport);
	surface = SDL_CreateRGBSurface(0, viewport.w, viewport.h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		0x00FF0000, 0x0000FF00, 0x000000FF,
#else
		0x000000FF, 0x0000FF00, 0x00FF0000,
#endif
		0x00000000);
	if (!surface) {
		SDL_Log("Couldn't create surface: %s\n", SDL_GetError());
		return;
	}

	if (SDL_RenderReadPixels(renderer, NULL, surface->format->format,
		surface->pixels, surface->pitch) < 0) {
		SDL_Log("Couldn't read screen: %s\n", SDL_GetError());
		SDL_free(surface);
		return;
	}

	if (SDL_SaveBMP(surface, "screenshot.bmp") < 0) {
		SDL_Log("Couldn't save screenshot.bmp: %s\n", SDL_GetError());
		SDL_free(surface);
		return;
	}
}

static void
FullscreenTo(int index, int windowId)
{
	Uint32 flags;
	struct SDL_Rect rect = { 0, 0, 0, 0 };
	SDL_Window* window = SDL_GetWindowFromID(windowId);
	if (!window) {
		return;
	}

	SDL_GetDisplayBounds(index, &rect);

	flags = SDL_GetWindowFlags(window);
	if (flags & SDL_WINDOW_FULLSCREEN) {
		SDL_SetWindowFullscreen(window, SDL_FALSE);
		SDL_Delay(15);
	}

	SDL_SetWindowPosition(window, rect.x, rect.y);
	SDL_SetWindowFullscreen(window, SDL_TRUE);
}

void SDLTest_CommonEvent(SDLTest_CommonState* state, SDL_Event* event, int* done)
{
	int i;
	static SDL_MouseMotionEvent lastEvent;

	if (state->verbose & VERBOSE_EVENT) {
		SDLTest_PrintEvent(event);
	}

	switch (event->type) {
	case SDL_WINDOWEVENT:
		switch (event->window.event) {
		case SDL_WINDOWEVENT_CLOSE:
		{
			SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);
			if (window) {
				for (i = 0; i < state->num_windows; ++i) {
					if (window == state->windows[i]) {
						if (state->targets[i]) {
							SDL_DestroyTexture(state->targets[i]);
							state->targets[i] = NULL;
						}
						if (state->renderers[i]) {
							SDL_DestroyRenderer(state->renderers[i]);
							state->renderers[i] = NULL;
						}
						SDL_DestroyWindow(state->windows[i]);
						state->windows[i] = NULL;
						break;
					}
				}
			}
		}
		break;
		}
		break;
	case SDL_KEYDOWN: {
		SDL_bool withControl = !!(event->key.keysym.mod & KMOD_CTRL)?SDL_TRUE:SDL_FALSE;
		SDL_bool withShift = !!(event->key.keysym.mod & KMOD_SHIFT) ? SDL_TRUE : SDL_FALSE;;
		SDL_bool withAlt = !!(event->key.keysym.mod & KMOD_ALT) ? SDL_TRUE : SDL_FALSE;;

		switch (event->key.keysym.sym) {
			/* Add hotkeys here */
		case SDLK_PRINTSCREEN: {
			SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
			if (window) {
				for (i = 0; i < state->num_windows; ++i) {
					if (window == state->windows[i]) {
						SDLTest_ScreenShot(state->renderers[i]);
					}
				}
			}
		}
							 break;
		case SDLK_EQUALS:
			if (withControl) {
				/* Ctrl-+ double the size of the window */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					int w, h;
					SDL_GetWindowSize(window, &w, &h);
					SDL_SetWindowSize(window, w * 2, h * 2);
				}
			}
			break;
		case SDLK_MINUS:
			if (withControl) {
				/* Ctrl-- half the size of the window */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					int w, h;
					SDL_GetWindowSize(window, &w, &h);
					SDL_SetWindowSize(window, w / 2, h / 2);
				}
			}
			break;
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
			if (withAlt) {
				/* Alt-Up/Down/Left/Right switches between displays */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					int currentIndex = SDL_GetWindowDisplayIndex(window);
					int numDisplays = SDL_GetNumVideoDisplays();

					if (currentIndex >= 0 && numDisplays >= 1) {
						int dest;
						if (event->key.keysym.sym == SDLK_UP || event->key.keysym.sym == SDLK_LEFT) {
							dest = (currentIndex + numDisplays - 1) % numDisplays;
						}
						else {
							dest = (currentIndex + numDisplays + 1) % numDisplays;
						}
						SDL_Log("Centering on display %d\n", dest);
						SDL_SetWindowPosition(window,
							SDL_WINDOWPOS_CENTERED_DISPLAY(dest),
							SDL_WINDOWPOS_CENTERED_DISPLAY(dest));
					}
				}
			}
			if (withShift) {
				/* Shift-Up/Down/Left/Right shift the window by 100px */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					const int delta = 100;
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);

					if (event->key.keysym.sym == SDLK_UP)    y -= delta;
					if (event->key.keysym.sym == SDLK_DOWN)  y += delta;
					if (event->key.keysym.sym == SDLK_LEFT)  x -= delta;
					if (event->key.keysym.sym == SDLK_RIGHT) x += delta;

					SDL_Log("Setting position to (%d, %d)\n", x, y);
					SDL_SetWindowPosition(window, x, y);
				}
			}
			break;
		case SDLK_o:
			if (withControl) {
				/* Ctrl-O (or Ctrl-Shift-O) changes window opacity. */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					float opacity;
					if (SDL_GetWindowOpacity(window, &opacity) == 0) {
						if (withShift) {
							opacity += 0.20f;
						}
						else {
							opacity -= 0.20f;
						}
						SDL_SetWindowOpacity(window, opacity);
					}
				}
			}
			break;

		case SDLK_c:
			if (withControl) {
				/* Ctrl-C copy awesome text! */
				SDL_SetClipboardText("SDL rocks!\nYou know it!");
				printf("Copied text to clipboard\n");
			}
			if (withAlt) {
				/* Alt-C toggle a render clip rectangle */
				for (i = 0; i < state->num_windows; ++i) {
					int w, h;
					if (state->renderers[i]) {
						SDL_Rect clip;
						SDL_GetWindowSize(state->windows[i], &w, &h);
						SDL_RenderGetClipRect(state->renderers[i], &clip);
						if (SDL_RectEmpty(&clip)) {
							clip.x = w / 4;
							clip.y = h / 4;
							clip.w = w / 2;
							clip.h = h / 2;
							SDL_RenderSetClipRect(state->renderers[i], &clip);
						}
						else {
							SDL_RenderSetClipRect(state->renderers[i], NULL);
						}
					}
				}
			}
			if (withShift) {
				SDL_Window* current_win = SDL_GetKeyboardFocus();
				if (current_win) {
					const SDL_bool shouldCapture = (SDL_GetWindowFlags(current_win) & SDL_WINDOW_MOUSE_CAPTURE) == 0 ? SDL_TRUE : SDL_FALSE;;
					const int rc = SDL_CaptureMouse(shouldCapture);
					SDL_Log("%sapturing mouse %s!\n", shouldCapture ? "C" : "Unc", (rc == 0) ? "succeeded" : "failed");
				}
			}
			break;
		case SDLK_v:
			if (withControl) {
				/* Ctrl-V paste awesome text! */
				char* text = SDL_GetClipboardText();
				if (*text) {
					printf("Clipboard: %s\n", text);
				}
				else {
					printf("Clipboard is empty\n");
				}
				SDL_free(text);
			}
			break;
		case SDLK_g:
			if (withControl) {
				/* Ctrl-G toggle grab */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					SDL_SetWindowGrab(window, !SDL_GetWindowGrab(window) ? SDL_TRUE : SDL_FALSE);
				}
			}
			break;
		case SDLK_m:
			if (withControl) {
				/* Ctrl-M maximize */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					Uint32 flags = SDL_GetWindowFlags(window);
					if (flags & SDL_WINDOW_MAXIMIZED) {
						SDL_RestoreWindow(window);
					}
					else {
						SDL_MaximizeWindow(window);
					}
				}
			}
			break;
		case SDLK_r:
			if (withControl) {
				/* Ctrl-R toggle mouse relative mode */
				SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode() ? SDL_TRUE : SDL_FALSE);
			}
			break;
		case SDLK_z:
			if (withControl) {
				/* Ctrl-Z minimize */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					SDL_MinimizeWindow(window);
				}
			}
			break;
		case SDLK_RETURN:
			if (withControl) {
				/* Ctrl-Enter toggle fullscreen */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					Uint32 flags = SDL_GetWindowFlags(window);
					if (flags & SDL_WINDOW_FULLSCREEN) {
						SDL_SetWindowFullscreen(window, SDL_FALSE);
					}
					else {
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
					}
				}
			}
			else if (withAlt) {
				/* Alt-Enter toggle fullscreen desktop */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					Uint32 flags = SDL_GetWindowFlags(window);
					if (flags & SDL_WINDOW_FULLSCREEN) {
						SDL_SetWindowFullscreen(window, SDL_FALSE);
					}
					else {
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
				}
			}
			else if (withShift) {
				/* Shift-Enter toggle fullscreen desktop / fullscreen */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					Uint32 flags = SDL_GetWindowFlags(window);
					if ((flags & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP) {
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
					}
					else {
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
				}
			}

			break;
		case SDLK_b:
			if (withControl) {
				/* Ctrl-B toggle window border */
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				if (window) {
					const Uint32 flags = SDL_GetWindowFlags(window);
					const SDL_bool b = ((flags & SDL_WINDOW_BORDERLESS) != 0) ? SDL_TRUE : SDL_FALSE;
					SDL_SetWindowBordered(window, b);
				}
			}
			break;
		case SDLK_a:
			if (withControl) {
				/* Ctrl-A reports absolute mouse position. */
				int x, y;
				const Uint32 mask = SDL_GetGlobalMouseState(&x, &y);
				SDL_Log("ABSOLUTE MOUSE: (%d, %d)%s%s%s%s%s\n", x, y,
					(mask & SDL_BUTTON_LMASK) ? " [LBUTTON]" : "",
					(mask & SDL_BUTTON_MMASK) ? " [MBUTTON]" : "",
					(mask & SDL_BUTTON_RMASK) ? " [RBUTTON]" : "",
					(mask & SDL_BUTTON_X1MASK) ? " [X2BUTTON]" : "",
					(mask & SDL_BUTTON_X2MASK) ? " [X2BUTTON]" : "");
			}
			break;
		case SDLK_0:
			if (withControl) {
				SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Test Message", "You're awesome!", window);
			}
			break;
		case SDLK_1:
			if (withControl) {
				FullscreenTo(0, event->key.windowID);
			}
			break;
		case SDLK_2:
			if (withControl) {
				FullscreenTo(1, event->key.windowID);
			}
			break;
		case SDLK_ESCAPE:
			*done = 1;
			break;
		case SDLK_SPACE:
		{
			char message[256];
			SDL_Window* window = SDL_GetWindowFromID(event->key.windowID);

			SDL_snprintf(message, sizeof(message), "(%i, %i), rel (%i, %i)\n", lastEvent.x, lastEvent.y, lastEvent.xrel, lastEvent.yrel);
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Last mouse position", message, window);
			break;
		}
		default:
			break;
		}
		break;
	}
	case SDL_QUIT:
		*done = 1;
		break;
	case SDL_MOUSEMOTION:
		lastEvent = event->motion;
		break;

	case SDL_DROPFILE:
	case SDL_DROPTEXT:
		SDL_free(event->drop.file);
		break;
	}
}


int done;
Uint32 frames;

#include"../Framework/Application.h"
GameEngine::Application gApplication;


void loop()
{
	SDL_Event event;
	int i;
	int status;
	//static SimpleRenderer simpleRenderer(&ctx);




	/* Check for events */
	++frames;
	while (SDL_PollEvent(&event) && !done) {
		switch (event.type) {
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				for (i = 0; i < state->num_windows; ++i) {
					if (event.window.windowID == SDL_GetWindowID(state->windows[i])) {
						int w, h;
						status = SDL_GL_MakeCurrent(state->windows[i], context[i]);
						if (status) {
							SDL_Log("SDL_GL_MakeCurrent(): %s\n", SDL_GetError());
							break;
						}
						/* Change view port to the new window dimensions */
						SDL_GL_GetDrawableSize(state->windows[i], &w, &h);
						ctx.glViewport(0, 0, w, h);
						state->window_w = event.window.data1;
						state->window_h = event.window.data2;
						/* Update window content */
						//Render(event.window.data1, event.window.data2, &datas[i]);
						//simpleRenderer.UpdateWindowSize(state->window_w, state->window_h);
						//simpleRenderer.Draw();

						SDL_GL_SwapWindow(state->windows[i]);
						break;
					}
				}
				break;
			}
		}
		SDLTest_CommonEvent(state, &event, &done);
	}

	if (!done) {

		gApplication.Update();
		for (i = 0; i < state->num_windows; ++i) {

			static int delta = 1;
			int x, y;
			SDL_GetWindowPosition(state->windows[i], &x, &y);

			if (x+state->window_w>= 1366)delta = -1;
			if (x <= 0)delta = 1;
				
			x += delta;
			//y += delta;
			
			SDL_SetWindowPosition(state->windows[i], x, y);


			status = SDL_GL_MakeCurrent(state->windows[i], context[i]);
			if (status) {
				SDL_Log("SDL_GL_MakeCurrent(): %s\n", SDL_GetError());

				/* Continue for next window */
				continue;
			}
			//Render(state->window_w, state->window_h, &datas[i]);
			//simpleRenderer.UpdateWindowSize(state->window_w, state->window_h);
			//simpleRenderer.Draw();
			gApplication.Draw();
			SDL_GL_SwapWindow(state->windows[i]);
		}
	}
#ifdef __EMSCRIPTEN__
	else {
		emscripten_cancel_main_loop();
	}
#endif
}
//
//int main(int argc, char** argv) {
//	int fsaa, accel;
//	int value;
//	int i;
//	SDL_DisplayMode mode;
//	Uint32 then, now;
//	int status;
//	//shader_data* data;
//
//
//
//	state = CommonCreateState(argv, SDL_INIT_VIDEO);
//	if (!state) {
//		return 1;
//	}
//
//	/* Set OpenGL parameters */
//	state->window_flags |= SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS;
//	state->gl_red_size = 5;
//	state->gl_green_size = 5;
//	state->gl_blue_size = 5;
//	state->gl_depth_size = depth;
//	state->gl_major_version = 3;
//	state->gl_minor_version = 0;
//	state->gl_profile_mask = SDL_GL_CONTEXT_PROFILE_ES;
//
//	if (!CommonInit(state)) {
//		quit(2);
//		return 0;
//	}
//
//	context = (SDL_GLContext*)SDL_calloc(state->num_windows, sizeof(context));
//	if (context == NULL) {
//		SDL_Log("Out of memory!\n");
//		quit(2);
//	}
//
//	/* Create OpenGL ES contexts */
//	for (i = 0; i < state->num_windows; i++) {
//		context[i] = SDL_GL_CreateContext(state->windows[i]);
//		if (!context[i]) {
//			SDL_Log("SDL_GL_CreateContext(): %s\n", SDL_GetError());
//			quit(2);
//		}
//	}
//
//	/* Important: call this *after* creating the context */
//	if (LoadContext(&ctx) < 0) {
//		SDL_Log("Could not load GLES2 functions\n");
//		quit(2);
//		return 0;
//	}
//
//
//
//
//
//
//
//
//	/* Initialize parameters */
//	fsaa = 0;
//	accel = 0;
//	if (state->render_flags & SDL_RENDERER_PRESENTVSYNC) {
//		SDL_GL_SetSwapInterval(1);
//	}
//	else {
//		SDL_GL_SetSwapInterval(0);
//	}
//
//	SDL_GetCurrentDisplayMode(0, &mode);
//	SDL_Log("Screen bpp: %d\n", SDL_BITSPERPIXEL(mode.format));
//	SDL_Log("\n");
//	SDL_Log("Vendor     : %s\n", ctx.glGetString(GL_VENDOR));
//	SDL_Log("Renderer   : %s\n", ctx.glGetString(GL_RENDERER));
//	SDL_Log("Version    : %s\n", ctx.glGetString(GL_VERSION));
//	//SDL_Log("Extensions : %s\n", ctx.glGetString(GL_EXTENSIONS));
//	SDL_Log("\n");
//
//	status = SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
//	if (!status) {
//		SDL_Log("SDL_GL_RED_SIZE: requested %d, got %d\n", 5, value);
//	}
//	else {
//		SDL_Log("Failed to get SDL_GL_RED_SIZE: %s\n",
//			SDL_GetError());
//	}
//	status = SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
//	if (!status) {
//		SDL_Log("SDL_GL_GREEN_SIZE: requested %d, got %d\n", 5, value);
//	}
//	else {
//		SDL_Log("Failed to get SDL_GL_GREEN_SIZE: %s\n",
//			SDL_GetError());
//	}
//	status = SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
//	if (!status) {
//		SDL_Log("SDL_GL_BLUE_SIZE: requested %d, got %d\n", 5, value);
//	}
//	else {
//		SDL_Log("Failed to get SDL_GL_BLUE_SIZE: %s\n",
//			SDL_GetError());
//	}
//	status = SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
//	if (!status) {
//		SDL_Log("SDL_GL_DEPTH_SIZE: requested %d, got %d\n", depth, value);
//	}
//	else {
//		SDL_Log("Failed to get SDL_GL_DEPTH_SIZE: %s\n",
//			SDL_GetError());
//	}
//	if (fsaa) {
//		status = SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &value);
//		if (!status) {
//			SDL_Log("SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value);
//		}
//		else {
//			SDL_Log("Failed to get SDL_GL_MULTISAMPLEBUFFERS: %s\n",
//				SDL_GetError());
//		}
//		status = SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value);
//		if (!status) {
//			SDL_Log("SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa,
//				value);
//		}
//		else {
//			SDL_Log("Failed to get SDL_GL_MULTISAMPLESAMPLES: %s\n",
//				SDL_GetError());
//		}
//	}
//	if (accel) {
//		status = SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &value);
//		if (!status) {
//			SDL_Log("SDL_GL_ACCELERATED_VISUAL: requested 1, got %d\n", value);
//		}
//		else {
//			SDL_Log("Failed to get SDL_GL_ACCELERATED_VISUAL: %s\n",
//				SDL_GetError());
//		}
//	}
//
//
//
//
//	/* Set rendering settings for each context */
//	for (i = 0; i < state->num_windows; ++i) {
//
//		int w, h;
//		status = SDL_GL_MakeCurrent(state->windows[i], context[i]);
//		if (status) {
//			SDL_Log("SDL_GL_MakeCurrent(): %s\n", SDL_GetError());
//
//			/* Continue for next window */
//			continue;
//		}
//		SDL_GL_GetDrawableSize(state->windows[i], &w, &h);
//		ctx.glViewport(0, 0, w, h);
//		GL_CHECK(ctx.glEnable(GL_CULL_FACE));
//		GL_CHECK(ctx.glEnable(GL_DEPTH_TEST));
//	}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//	gApplication.Init(state->window_w, state->window_h);
//
//	/* Main render loop */
//	frames = 0;
//	then = SDL_GetTicks();
//	done = 0;
//#ifdef __EMSCRIPTEN__
//	emscripten_set_main_loop(loop, 0, 1);
//#else
//	while (!done) {
//		loop();
//	}
//#endif
//
//	gApplication.Cleanup();
//	/* Print out some timing information */
//	now = SDL_GetTicks();
//	if (now > then) {
//		SDL_Log("%2.2f frames per second\n",
//			((double)frames * 1000) / (now - then));
//	}
//
//#if !defined(__ANDROID__) && !defined(__NACL__)  
//	quit(0);
//#endif    
//	return 0;
//}
//

int SDL_main(int agrv, char** argc) {
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	int width = mode.w;
	int height = mode.h;
	SDL_Log("width %d, height %d", width, height);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	SDL_Window* window = SDL_CreateWindow("Name",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 960, 640,
		SDL_WINDOW_OPENGL | //SDL_WINDOW_FULLSCREEN |
		SDL_WINDOW_RESIZABLE);
	SDL_GLContext gl = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(0);

	//if (state->render_flags & SDL_RENDERER_PRESENTVSYNC) {
	//}
	//else {
	//	SDL_GL_SetSwapInterval(0);
	//}
	LoadContext(&ctx);
	SimpleRenderer simpleRenderer(&ctx);

	SDL_Event event;
	bool running = true;
	long int currentTime = SDL_GetTicks();
	long int lastTime = 0;
	float deltaTime;
	while (running) {
		// lastTime = currentTime;
		// currentTime = SDL_GetTicks();
		// deltaTime = (float)(currentTime - lastTime)/1000.0f;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				running = false;
			// if(event.type == SDL_MOUSEBUTTONDOWN){
			// 	s_inputDispatcher.SendEvent(SDL_MOUSEBUTTONDOWN,event.motion.x,event.motion.y);
			// }
			// if(event.type == SDL_MOUSEBUTTONUP){
			// 	s_inputDispatcher.SendEvent(SDL_MOUSEBUTTONUP,event.motion.x,event.motion.y);
			// }
			// if(event.type == SDL_MOUSEMOTION){
			// 	s_inputDispatcher.SendEvent(SDL_MOUSEMOTION,event.motion.x,event.motion.y);
			// 	//SDL_Log("%d %d",event.motion.x,event.motion.y);
			// }


			if (event.type == SDL_MOUSEMOTION) {
				//gameBase->HandleEvent(InputEvent(EventType::MOUSE_MOTION, event.motion.x, event.motion.y));
			}
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				//gameBase->HandleEvent(InputEvent(EventType::MOUSE_BUTTON_DOWN, event.motion.x, event.motion.y));
			}
			if (event.type == SDL_MOUSEBUTTONUP) {
				//gameBase->HandleEvent(InputEvent(EventType::MOUSE_BUTTON_UP, event.motion.x, event.motion.y));
			}
		}
		//gameBase->Update();
		//gameBase->Draw();
		int w, h;
		SDL_GL_GetDrawableSize(window, &w, &h);
		simpleRenderer.UpdateWindowSize(w,h);
		simpleRenderer.Draw();
		SDL_GL_SwapWindow(window);
		//if (gameBase->Done())
			//running = false;
	}
	//gameBase->CleanUp();
	//delete gameBase;

	SDL_GL_DeleteContext(gl);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
