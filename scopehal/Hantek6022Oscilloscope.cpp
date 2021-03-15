/***********************************************************************************************************************
*                                                                                                                      *
* ANTIKERNEL v0.1                                                                                                      *
*                                                                                                                      *
* Copyright (c) 2012-2020 Andrew D. Zonenberg                                                                          *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief Implementation of Hantek6022Oscilloscope
 */

#include "scopehal.h"
#include "OscilloscopeChannel.h"
#include "Hantek6022Oscilloscope.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

Hantek6022Oscilloscope::Hantek6022Oscilloscope(const string& name, const string& vendor, const string& serial)
	: m_name(name)
	, m_vendor(vendor)
	, m_serial(serial)
	, m_extTrigger(NULL)
{
}

Hantek6022Oscilloscope::~Hantek6022Oscilloscope()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Information queries

string Hantek6022Oscilloscope::IDPing()
{
	return "";
}

string Hantek6022Oscilloscope::GetTransportName()
{
	return "null";
}

string Hantek6022Oscilloscope::GetTransportConnectionString()
{
	return "";
}

string Hantek6022Oscilloscope::GetDriverNameInternal()
{
	return "mock";
}

unsigned int Hantek6022Oscilloscope::GetInstrumentTypes()
{
	return INST_OSCILLOSCOPE;
}

string Hantek6022Oscilloscope::GetName()
{
	return m_name;
}

string Hantek6022Oscilloscope::GetVendor()
{
	return m_vendor;
}

string Hantek6022Oscilloscope::GetSerial()
{
	return m_serial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Triggering

Oscilloscope::TriggerMode Hantek6022Oscilloscope::PollTrigger()
{
	//we never trigger
	return TRIGGER_MODE_STOP;
}

bool Hantek6022Oscilloscope::AcquireData()
{
	//no new data possible
	return false;
}

void Hantek6022Oscilloscope::ArmTrigger()
{
	//no-op, we never trigger
}

void Hantek6022Oscilloscope::StartSingleTrigger()
{
	//no-op, we never trigger
}

void Hantek6022Oscilloscope::Start()
{
	//no-op, we never trigger
}

void Hantek6022Oscilloscope::Stop()
{
	//no-op, we never trigger
}

bool Hantek6022Oscilloscope::IsTriggerArmed()
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization

void Hantek6022Oscilloscope::LoadConfiguration(const YAML::Node& node, IDTable& table)
{
	//Load the channels
	auto& chans = node["channels"];
	for(auto it : chans)
	{
		auto& cnode = it.second;

		//Allocate channel space if we didn't have it yet
		size_t index = cnode["index"].as<int>();
		if(m_channels.size() < (index+1))
			m_channels.resize(index+1);

		//Configure the channel
		OscilloscopeChannel::ChannelType type = OscilloscopeChannel::CHANNEL_TYPE_COMPLEX;
		string stype = cnode["type"].as<string>();
		if(stype == "analog")
			type = OscilloscopeChannel::CHANNEL_TYPE_ANALOG;
		else if(stype == "digital")
			type = OscilloscopeChannel::CHANNEL_TYPE_DIGITAL;
		else if(stype == "trigger")
			type = OscilloscopeChannel::CHANNEL_TYPE_TRIGGER;
		auto chan = new OscilloscopeChannel(
			this,
			cnode["name"].as<string>(),
			type,
			cnode["color"].as<string>(),
			1,
			index,
			true);
		m_channels[index] = chan;

		//Create the channel ID
		table.emplace(cnode["id"].as<int>(), chan);
	}

	//Call the base class to configure everything
	Oscilloscope::LoadConfiguration(node, table);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Channel configuration. Mostly trivial stubs.

bool Hantek6022Oscilloscope::IsChannelEnabled(size_t i)
{
	return m_channelsEnabled[i];
}

void Hantek6022Oscilloscope::EnableChannel(size_t i)
{
	m_channelsEnabled[i] = true;
}

void Hantek6022Oscilloscope::DisableChannel(size_t i)
{
	m_channelsEnabled[i] = false;
}

OscilloscopeChannel::CouplingType Hantek6022Oscilloscope::GetChannelCoupling(size_t i)
{
	return m_channelCoupling[i];
}

void Hantek6022Oscilloscope::SetChannelCoupling(size_t i, OscilloscopeChannel::CouplingType type)
{
	m_channelCoupling[i] = type;
}

double Hantek6022Oscilloscope::GetChannelAttenuation(size_t i)
{
	return m_channelAttenuation[i];
}

void Hantek6022Oscilloscope::SetChannelAttenuation(size_t i, double atten)
{
	m_channelAttenuation[i] = atten;
}

int Hantek6022Oscilloscope::GetChannelBandwidthLimit(size_t i)
{
	return m_channelBandwidth[i];
}

void Hantek6022Oscilloscope::SetChannelBandwidthLimit(size_t i, unsigned int limit_mhz)
{
	m_channelBandwidth[i] = limit_mhz;
}

double Hantek6022Oscilloscope::GetChannelVoltageRange(size_t i)
{
	return m_channelVoltageRange[i];
}

void Hantek6022Oscilloscope::SetChannelVoltageRange(size_t i, double range)
{
	m_channelVoltageRange[i] = range;
}

OscilloscopeChannel* Hantek6022Oscilloscope::GetExternalTrigger()
{
	return m_extTrigger;
}

double Hantek6022Oscilloscope::GetChannelOffset(size_t i)
{
	return m_channelOffset[i];
}

void Hantek6022Oscilloscope::SetChannelOffset(size_t i, double offset)
{
	m_channelOffset[i] = offset;
}

vector<uint64_t> Hantek6022Oscilloscope::GetSampleRatesNonInterleaved()
{
	//no-op
	vector<uint64_t> ret;
	return ret;
}

vector<uint64_t> Hantek6022Oscilloscope::GetSampleRatesInterleaved()
{
	//no-op
	vector<uint64_t> ret;
	return ret;
}

set<Oscilloscope::InterleaveConflict> Hantek6022Oscilloscope::GetInterleaveConflicts()
{
	//no-op
	set<Oscilloscope::InterleaveConflict> ret;
	return ret;
}

vector<uint64_t> Hantek6022Oscilloscope::GetSampleDepthsNonInterleaved()
{
	//no-op
	vector<uint64_t> ret;
	return ret;
}

vector<uint64_t> Hantek6022Oscilloscope::GetSampleDepthsInterleaved()
{
	//no-op
	vector<uint64_t> ret;
	return ret;
}

uint64_t Hantek6022Oscilloscope::GetSampleRate()
{
	return 1;
}

uint64_t Hantek6022Oscilloscope::GetSampleDepth()
{
	//FIXME
	return 1;
}

void Hantek6022Oscilloscope::SetSampleDepth(uint64_t /*depth*/)
{
	//no-op
}

void Hantek6022Oscilloscope::SetSampleRate(uint64_t /*rate*/)
{
	//no-op
}

void Hantek6022Oscilloscope::SetTriggerOffset(int64_t /*offset*/)
{
	//FIXME
}

int64_t Hantek6022Oscilloscope::GetTriggerOffset()
{
	//FIXME
	return 0;
}

bool Hantek6022Oscilloscope::IsInterleaving()
{
	return false;
}

bool Hantek6022Oscilloscope::SetInterleaving(bool /*combine*/)
{
	return false;
}

void Hantek6022Oscilloscope::PushTrigger()
{
	//no-op
}

void Hantek6022Oscilloscope::PullTrigger()
{
	//no-op
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Import a waveform from CSV

bool Hantek6022Oscilloscope::LoadCSV(const string& path)
{
	LogTrace("Importing CSV file \"%s\"\n", path.c_str());
	LogIndenter li;

	FILE* fp = fopen(path.c_str(), "r");
	if(!fp)
	{
		LogError("Failed to open file\n");
		return false;
	}

	vector<AnalogWaveform*> waveforms;

	char line[1024];
	size_t nrow = 0;
	size_t ncols = 0;
	vector<string> channel_names;
	while(!feof(fp))
	{
		nrow ++;

		if(!fgets(line, sizeof(line), fp))
			break;

		//Parse the samples for each row
		//TODO: be more efficient about this
		vector<float> row;
		string tmp;
		for(size_t i=0; i < sizeof(line); i++)
		{
			if(line[i] == '\0' || line[i] == ',')
			{
				float f;
				sscanf(tmp.c_str(), "%f", &f);
				row.push_back(f);

				if(line[i] == '\0')
					break;
				else
					tmp = "";
			}
			else
				tmp += line[i];
		}

		//If this is the first line, figure out how many columns we have.
		//First column is always timestamp in seconds.
		//TODO: support timestamp in abstract sample units instead
		if(nrow == 1)
		{
			ncols = row.size() - 1;

			//See if the first row is numeric
			bool numeric = true;
			for(size_t i=0; (i<sizeof(line)) && (line[i] != '\0'); i++)
			{
				if(!isdigit(line[i]) && !isspace(line[i]) && (line[i] != ',') && (line[i] != '.') )
				{
					numeric = false;
					break;
				}
			}

			if(!numeric)
			{
				LogTrace("Found %zu signal columns, with header row\n", ncols);

				//Extract names of the headers
				tmp = "";
				for(size_t i=0; i < sizeof(line); i++)
				{
					if(line[i] == '\0' || line[i] == ',')
					{
						channel_names.push_back(tmp);

						if(line[i] == '\0')
							break;
						else
							tmp = "";
					}
					else
						tmp += line[i];
				}

				//Discard name of timestamp column
				channel_names.erase(channel_names.begin());

				continue;
			}

			else
			{
				for(size_t i=0; i<ncols; i++)
					channel_names.push_back(string("CH") + to_string(i+1));

				LogTrace("Found %zu signal columns, no header row\n", ncols);
			}
		}

		//If we don't have any channels, create them
		if(GetChannelCount() == 0)
		{
			//Create the columns
			for(size_t i=0; i<ncols; i++)
			{
				//Create the channel
				auto chan = new OscilloscopeChannel(
					this,
					channel_names[i],
					OscilloscopeChannel::CHANNEL_TYPE_ANALOG,
					GetDefaultChannelColor(i),
					1,
					i,
					true);
				AddChannel(chan);
				chan->SetDefaultDisplayName();
			}
		}

		//Create waveforms if needed
		if(waveforms.empty())
		{
			for(size_t i=0; i<ncols; i++)
			{
				//Create the waveform for the channel
				auto wfm = new AnalogWaveform;
				wfm->m_timescale = 1;
				wfm->m_startTimestamp = 0;
				wfm->m_startFemtoseconds = 0;
				wfm->m_triggerPhase = 0;
				waveforms.push_back(wfm);
				GetChannel(i)->SetData(wfm, 0);
			}
		}

		int64_t timestamp = row[0] * FS_PER_SECOND;
		for(size_t i=0; i<ncols; i++)
		{
			if(i+1 >= row.size())
				break;

			auto w = waveforms[i];
			w->m_offsets.push_back(timestamp);
			w->m_samples.push_back(row[i+1]);

			//Extend last sample
			if(!w->m_durations.empty())
			{
				size_t last = w->m_durations.size() - 1;
				w->m_durations[last] = timestamp - w->m_offsets[last];
			}

			//Add duration for this sample
			w->m_durations.push_back(1);
		}
	}

	fclose(fp);

	//Calculate gain/offset for each channel
	for(size_t i=0; i<ncols; i++)
	{
		float vmin = FLT_MAX;
		float vmax = -FLT_MAX;

		for(auto v : waveforms[i]->m_samples)
		{
			vmax = max(vmax, (float)v);
			vmin = min(vmin, (float)v);
		}

		//LogDebug("vmax = %f, vmin = %f\n", vmax, vmin);

		auto chan = GetChannel(i);
		chan->SetVoltageRange(vmax - vmin);
		chan->SetOffset((vmin-vmax) / 2);
	}

	return true;
}
