#include "SoundClass.h"

SoundClass::SoundClass()
	: m_DirectSound(0)
	, m_primaryBuffer(0)
	, m_secondaryBuffer1(0)
{}

SoundClass::SoundClass(const SoundClass& other) {}
SoundClass::~SoundClass() {}

bool SoundClass::Initialize(HWND hWnd)
{
	bool result;

	result = InitializeDirectSound(hWnd);
	if (!result) return false;

	result = LoadWaveFile("../Engine/data/goldn.wav", &m_secondaryBuffer1);
	if (!result) return false;

	result = PlayWaveFile();
	if (!result) return false;

	return true;
}

void SoundClass::Shutdown()
{
	ShutdownWaveFile(&m_secondaryBuffer1);
	ShutdownDirectSound();
}

bool SoundClass::InitializeDirectSound(HWND hWnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	// �⺻ ���� ��ġ�� �޾ƿɴϴ�.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result)) return false;

	// ���α׷��Ӱ� ���� ī�忡 ������ �� �ֵ��� �����Ѵ�.
	result = m_DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if (FAILED(result)) return false;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 1�� ���ۿ� �⺻ ���� ��ġ�� ������ �� �ִ� ������ ������ �մϴ�.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result)) return false;

	// ����� CD ����� ǰ���� �����մϴ�.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result)) return false;

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	if (m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}
}

bool SoundClass::LoadWaveFile(char* path, IDirectSoundBuffer8** secondaryBuffer)
{
	int error;
	FILE* fp;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	error = fopen_s(&fp, path, "rb");
	if (error != 0) return false;

	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp);
	if (count != 1) return false;

	// �ҷ��� ����� ûũ�� RIFF �������� Ȯ���մϴ�.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// �ҷ��� ����� ������ WAVE �������� Ȯ���մϴ�.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// �ҷ��� ����� ���� ûũ�� fmt �������� Ȯ���մϴ�.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// ����� ������ PCM ������� Ȯ���մϴ�.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM) return false;

	// ���׷��� �������� ����Ǿ����� Ȯ���մϴ�.
	if (waveFileHeader.numChannels != 2) return false;

	// ���� ����Ʈ�� 44.1 �츣������ Ȯ���մϴ�.
	if (waveFileHeader.sampleRate != 44100) return false;

	// 16��Ʈ �������� Ȯ���մϴ�.
	if (waveFileHeader.bitsPerSample != 16) return false;

	waveFileHeader.dummy;

	// ������ ûũ�� data ���� Ȯ���մϴ�.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// IDirectSoundBuffer ��ü�� ���� �����մϴ�.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result)) return false;

	// ������ IDirectSoundBuffer ��ü�� 2�� ���۸� �����մϴ�.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result)) return false;

	tempBuffer->Release();
	tempBuffer = 0;
	
	// ����� �ǳʶ� �κ����� ���� Ŀ���� �ű�ϴ�.
	fseek(fp, sizeof(WaveHeaderType), SEEK_SET);

	// ���� �����͸� ��Ƶ� �ӽ� ���۸� ����ϴ�.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData) return false;

	// 1����Ʈ�� ������ ũ�⸸ŭ �ӽ� ���ۿ� �н��ϴ�.
	count = fread(waveData, 1, waveFileHeader.dataSize, fp);
	if (count != waveFileHeader.dataSize) return false;

	// ������ �ݽ��ϴ�.
	error = fclose(fp);
	if (error != 0) return false;

	// �����͸� �ֱ� ���� 2�� ���۸� ����ϴ�.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result)) return false;

	// ������ ũ�⸸ŭ ���۸� �����մϴ�.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// 2�� ���� ����� �����մϴ�.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result)) return false;

	// �����͸� �ӽ� �����ϱ� ���� ����� �ӽ� ���۸� �����մϴ�.
	delete[] waveData;
	waveData = 0;

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		(*secondaryBuffer) = 0;
	}
}

bool SoundClass::PlayWaveFile()
{
	HRESULT result;

	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if (FAILED(result)) return false;

	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result)) return false;

	result = m_secondaryBuffer1->Play(0, 0, 0);
	if (FAILED(result)) return false;

	return true;
}
