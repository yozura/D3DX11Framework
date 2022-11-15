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

	// 기본 음향 장치를 받아옵니다.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result)) return false;

	// 프로그래머가 사운드 카드에 접근할 수 있도록 설정한다.
	result = m_DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if (FAILED(result)) return false;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// 1차 버퍼에 기본 음향 장치를 제어할 수 있는 권한을 갖도록 합니다.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result)) return false;

	// 비압축 CD 오디오 품질로 설정합니다.
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

	// 불러온 오디오 청크가 RIFF 포맷인지 확인합니다.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// 불러온 오디오 형식이 WAVE 포맷인지 확인합니다.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// 불러온 오디오 서브 청크가 fmt 포맷인지 확인합니다.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// 오디오 포맷이 PCM 방식인지 확인합니다.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM) return false;

	// 스테레오 형식으로 저장되었는지 확인합니다.
	if (waveFileHeader.numChannels != 2) return false;

	// 샘플 레이트가 44.1 헤르츠인지 확인합니다.
	if (waveFileHeader.sampleRate != 44100) return false;

	// 16비트 형식인지 확인합니다.
	if (waveFileHeader.bitsPerSample != 16) return false;

	waveFileHeader.dummy;

	// 데이터 청크가 data 인지 확인합니다.
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

	// IDirectSoundBuffer 객체를 먼저 생성합니다.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result)) return false;

	// 생성한 IDirectSoundBuffer 객체로 2차 버퍼를 생성합니다.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result)) return false;

	tempBuffer->Release();
	tempBuffer = 0;
	
	// 헤더를 건너뛴 부분으로 파일 커서를 옮깁니다.
	fseek(fp, sizeof(WaveHeaderType), SEEK_SET);

	// 파일 데이터를 모아둘 임시 버퍼를 만듭니다.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData) return false;

	// 1바이트씩 데이터 크기만큼 임시 버퍼에 읽습니다.
	count = fread(waveData, 1, waveFileHeader.dataSize, fp);
	if (count != waveFileHeader.dataSize) return false;

	// 파일을 닫습니다.
	error = fclose(fp);
	if (error != 0) return false;

	// 데이터를 넣기 위해 2차 버퍼를 잠굽니다.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result)) return false;

	// 데이터 크기만큼 버퍼를 복사합니다.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// 2차 버퍼 잠금을 해제합니다.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result)) return false;

	// 데이터를 임시 저장하기 위해 사용한 임시 버퍼를 해제합니다.
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
