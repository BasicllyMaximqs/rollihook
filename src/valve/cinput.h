typedef unsigned int CRC32_t;
class CInput
{
public:
	class CVerifiedUserCmd
	{
	public:
		CUserCmd	m_cmd;
		CRC32_t		m_crc;
	};
public:
	std::byte			pad0[0xC];				// 0x00
	bool				bTrackIRAvailable;		// 0x0C
	bool				bMouseInitialized;		// 0x0D
	bool				bMouseActive;			// 0x0E
	std::byte			pad1[0x9A];				// 0x0F
	bool				bCameraInThirdPerson;	// 0xA9
	std::byte			pad2[0x2];				// 0xAA
	CVector				vecCameraOffset;		// 0xAC
	std::byte			pad3[0x38];				// 0xB8
};