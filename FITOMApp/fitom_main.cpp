#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <gdiplus.h>
#include <WinSock2.h>

#include "FITOM.h"
#include "FITOMdefine.h"
#include "SoundDev.h"
#include "FITOMApp.h"
#include "FITOMAppDlg.h"
#include "FITOMCfg.h"
#include "SCCIWrapper.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "codec.h"
#include "ADPCM.h"
#include "SplashDlg.h"
#include "ProgressDlg.h"
#include "FTSPI.h"

