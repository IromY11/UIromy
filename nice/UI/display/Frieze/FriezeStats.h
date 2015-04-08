#ifndef _ITF_FRIEZESTATS_H_
#define _ITF_FRIEZESTATS_H_

#ifndef ITF_FINAL
    #define ENABLE_FRIEZESTATS
#endif


namespace ITF
{
#ifdef ENABLE_FRIEZESTATS
    class Frise;

    class FriezeStats
    {
    public:
        typedef struct VB
        {
            i32         m_vertexCount;
            ObjectRef   m_owner;
        } VB;

        static FriezeStats* m_singleton;

        static void createSingleton();
        static void deleteSingleton();
        void     addFriezeInstance(ObjectRef _or);
        void     removeFriezeInstance(ObjectRef _or);
        void     addFriezeVB(void* _adrs, ObjectRef _or, i32 _size);
        void     removeFriezeVB(void* _adrs);
        void     dumpToFile();

    private:
        SafeArray<ObjectRef> m_friezeList;
        ITF_MAP<uPtr, VB> m_vbList;
    };

#define FRIEZESTAT_INIT             FriezeStats::createSingleton()
#define FRIEZESTAT_STOP             FriezeStats::deleteSingleton()
#define FRIEZESTAT_ADD_FRIEZE(ref)  FriezeStats::m_singleton->addFriezeInstance(ref)
#define FRIEZESTAT_DEL_FRIEZE(ref)  FriezeStats::m_singleton->removeFriezeInstance(ref)
#define FRIEZESTAT_ADD_VB(adrs, ref, size)  FriezeStats::m_singleton->addFriezeVB((void*)(adrs), ref, size)
#define FRIEZESTAT_DEL_VB(adrs)  FriezeStats::m_singleton->removeFriezeVB((void*)(adrs))
#define FRIEZESTAT_DUMP             FriezeStats::m_singleton->dumpToFile();


#else // ENABLE_FRIEZESTATS


#define FRIEZESTAT_INIT             ((void)0)
#define FRIEZESTAT_STOP             ((void)0)
#define FRIEZESTAT_ADD_FRIEZE(ref)  ((void)0)
#define FRIEZESTAT_DEL_FRIEZE(ref)  ((void)0)
#define FRIEZESTAT_ADD_VB(adrs, ref, size)  ((void)0)
#define FRIEZESTAT_DEL_VB(adrs)     ((void)0)
#define FRIEZESTAT_DUMP             ((void)0)


#endif //ENABLE_FRIEZESTATS

#endif  // _ITF_FRIEZESTATS_H_
} // namespace ITF
