#ifndef ENGINE_MACRO_H__
#define ENGINE_MACRO_H__

namespace Engine
{
	#define Msg_Box(str) MessageBox(NULL, TEXT(str), TEXT("System message"), MB_OK)
	#define BEGIN(NAMESPACE) namespace NAMESPACE {
	#define END }
	#define USING(NAMESPACE) using namespace NAMESPACE;

	#if defined ENGINE_EXPORTS
	#define ENGINE_DLL _declspec(dllexport)
	#else
	#define ENGINE_DLL _declspec(dllimport)
	#endif

	#define NO_COPY(CLASSNAME)							\
		private:										\
		CLASSNAME(const CLASSNAME&);					\
		CLASSNAME& operator = (const CLASSNAME&);		

	#define DECLARE_SINGLETON(CLASSNAME)				\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static void DestroyInstance( void );			

	#define IMPLEMENT_SINGLETON(CLASSNAME)				\
		CLASSNAME*	CLASSNAME::m_pInstance = NULL;		\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
			if(NULL == m_pInstance) {					\
				m_pInstance = new CLASSNAME;			\
			}											\
			return m_pInstance;							\
		}												\
		void CLASSNAME::DestroyInstance( void ) {		\
			if(NULL != m_pInstance)	{					\
				delete m_pInstance;						\
				m_pInstance = NULL;						\
			}											\
		}	
}

#endif // ENGINE_MACRO_H__
