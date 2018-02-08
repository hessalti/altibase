/** 
 *  Copyright (c) 1999~2017, Altibase Corp. and/or its affiliates. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License, version 3,
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef _O_SVM_MANAGER_H_
#define _O_SVM_MANAGER_H_ 1

#include <idu.h>
#include <iduMemPool.h>
#include <idp.h>
#include <smDef.h>
#include <svmDef.h>
#include <sctTableSpaceMgr.h>
#include <smu.h>

// svmManager�� �ʱ�ȭ �ϴ� ���
typedef enum
{
    // �Ϲ� STARTUP - ��ũ�� �����ͺ��̽� ���� ����
    SVM_MGR_INIT_MODE_NORMAL_STARTUP = 1,
    // CREATEDB���� - ���� ��ũ�� �����ͺ��̽� ������ �������� ����
    SVM_MGR_INIT_MODE_CREATEDB = 2
} svmMgrInitMode ;

// fillPCHEntry �Լ����� ������ �޸��� ���翩�θ� ����
typedef enum svmFillPCHOption {
    SVM_FILL_PCH_OP_NONE = 0 ,
    /** ������ �޸��� ������ PCH ���� Page�� �����Ѵ� */
    SVM_FILL_PCH_OP_COPY_PAGE = 1,
    /** ������ �޸��� �����͸� PCH ���� Page�� �����Ѵ� */
    SVM_FILL_PCH_OP_SET_PAGE = 2
} svmFillPCHOption ;


// allocNewExpandChunk �Լ��� �ɼ�
typedef enum svmAllocChunkOption 
{
    SVM_ALLOC_CHUNK_OP_NONE = 0,              // �ɼǾ���
    SVM_ALLOC_CHUNK_OP_CREATEDB         = 1   // createdb �������� �˸�
} svmAllocChunkOption ;

/*********************************************************************
  �޸� �����ͺ��̽��� ������ ���
 *********************************************************************   
  --------------------------------------------------
   Page#1  ~ Page#10  Expand Chunk #0
  --------------------------------------------------
   Page#11 ~ Page#20  Expand Chunk #1
  --------------------------------------------------
 *********************************************************************/

class svmManager
{
private:
    // �� �����ͺ��̽� Page�� non-durable�� �����͸� ���ϴ� PCH �迭
    static svmPCH            **mPCHArray[SC_MAX_SPACE_COUNT];

private :
    // �Ϲ� �޸� Page Pool�� �ʱ�ȭ�Ѵ�.
    static IDE_RC initializeDynMemPool(svmTBSNode * aTBSNode);
    
    // Tablespace�� �Ҵ�� Page Pool�� �ı��Ѵ�.
    static IDE_RC destroyPagePool( svmTBSNode * aTBSNode );

    // ���� �Ҵ�� Expand Chunk�ȿ� ���ϴ� Page���� PCH Entry�� �Ҵ��Ѵ�.
    // Chunk���� Free List Info Page�� Page Memory�� �Ҵ��Ѵ�.
    static IDE_RC fillPCHEntry4AllocChunk(svmTBSNode * aTBSNode,
                                          scPageID     aNewChunkFirstPID,
                                          scPageID     aNewChunkLastPID );
    
    static void dump(FILE     *a_fp,
                     scPageID  a_no);

    // Prepare-> Restore�� �������� Prepare�߿��� ���̴�
    // �ӽ� Page Memory�� �����ϴ� Memory Pool 
    static SChar * mPageBuffer4PrepareTBS; 
    static UInt  mPageBuffer4PrepareTBSSize;

    // Ư�� Page�� PCH���� Page Memory�� �Ҵ��Ѵ�.
    static IDE_RC allocPageMemory( svmTBSNode * aTBSNode,
                                   scPageID     aPID );

    // ������ �޸𸮸� �Ҵ��ϰ�, �ش� Page�� �ʱ�ȭ�Ѵ�.
    // �ʿ��� ���, ������ �ʱ�ȭ�� ���� �α��� �ǽ��Ѵ�
    static IDE_RC allocAndLinkPageMemory( svmTBSNode * aTBSNode,
                                          scPageID     aPID,
                                          scPageID     aPrevPID,
                                          scPageID     aNextPID );
    
    // Ư�� Page�� PCH���� Page Memory�� �����Ѵ�.
    static IDE_RC freePageMemory( svmTBSNode * aTBSNode, scPageID aPID );


    // FLI Page�� Next Free Page ID�� ��ũ�� �������鿡 ����
    // PCH���� Page �޸𸮸� �Ҵ��ϰ� Page Header�� Prev/Next�����͸� �����Ѵ�.
    static IDE_RC allocFreePageMemoryList( svmTBSNode * aTBSNode,
                                           scPageID     aHeadPID,
                                           scPageID     aTailPID,
                                           vULong     * aPageCount );

    // PCH�� Page���� Page Header�� Prev/Next�����͸� �������
    // FLI Page�� Next Free Page ID�� �����Ѵ�.
    static IDE_RC linkFreePageList( svmTBSNode * aTBSNode,
                                    void       * aHeadPage,
                                    void       * aTailPage,
                                    vULong     * aPageCount );
    
    /*
     * Page�� PCH(Page Control Header)������ �����Ѵ�.
     */
    // PCH �Ҵ�� �ʱ�ȭ
    static IDE_RC allocPCHEntry(svmTBSNode *  aTBSNode,
                                scPageID      a_pid);

    // PCH ����
    static IDE_RC freePCHEntry(svmTBSNode *  aTBSNode,
                               scPageID     a_pid);

    // ��� PCH �޸𸮸� �����Ѵ�.
    static IDE_RC freeAll(svmTBSNode * aTBSNode);

    // ������ ���̽��� Base Page (Page#0) �� ������ ������ �����Ѵ�.
    // �̿� ���õ� �����δ�  MemBase�� Catalog Table������ �ִ�.
    static IDE_RC setupBasePageInfo( svmTBSNode * aTBSNode, UChar *aBasePage );
    
    static IDE_RC freeFreePageMemoryList( svmTBSNode * aTBSNode,
                                          void       * aHeadPage,
                                          void       * aTailPage,
                                          vULong     * aPageCount );

    static IDE_RC allocPage( svmTBSNode * aTBSNode, svmTempPage ** aPage );

    static IDE_RC freePage( svmTBSNode * aTBSNode, svmTempPage * aPage );

public:
    
    /* --------------------
     * Class Control
     * -------------------*/
    svmManager();
    // svmManager�� �ʱ�ȭ�Ѵ�.
    static IDE_RC initializeStatic();
    static IDE_RC destroyStatic();

    // TBSNode�� �ʱ�ȭ�Ѵ�.
    static IDE_RC allocTBSNode(svmTBSNode        **aTBSNode,
                               smiTableSpaceAttr  *aTBSAttr);

    // TBSNode�� �����Ѵ�.
    static IDE_RC destroyTBSNode(svmTBSNode *aTBSNode);

    // Volatile TBS�� �ʱ�ȭ�Ѵ�.
    static IDE_RC initTBS(svmTBSNode *aTBSNode);

    // Volatile TBS�� �����Ѵ�.
    static IDE_RC finiTBS(svmTBSNode *aTBSNode);

    // Base Page ( 0�� Page ) �� Latch�� �Ǵ�
    static IDE_RC lockBasePage(svmTBSNode * aTBSNode);
    // Base Page ( 0�� Page ) ���� Latch�� Ǭ��.
    static IDE_RC unlockBasePage(svmTBSNode * aTBSNode);

    /* -----------------------
     * persistent  memory
     * manipulations
     * ----------------------*/
    
    // �������� Expand Chunk�� �߰��Ͽ� �����ͺ��̽��� Ȯ���Ѵ�.
    static IDE_RC allocNewExpandChunks( svmTBSNode *  aTBSNode,
                                        UInt          aExpandChunkCount );

    
    // Ư�� ������ ������ŭ �����ͺ��̽��� Ȯ���Ѵ�.
    static IDE_RC allocNewExpandChunk( svmTBSNode *  aTBSNode,
                                       scPageID      aNewChunkFirstPID,
                                       scPageID      aNewChunkLastPID);

    
    // DB�κ��� �ϳ��� Page�� �Ҵ�޴´�.
    static IDE_RC allocatePersPage (void       *  aTrans,
                                    scSpaceID     aSpaceID,
                                    void      **  aAllocatedPage);
    
    
    // DB�κ��� Page�� ������ �Ҵ�޾� Ʈ����ǿ��� Free Page�� �����Ѵ�.
    // aHeadPage���� aTailPage����
    // Page Header�� Prev/Next�����ͷ� �������ش�.
    static IDE_RC allocatePersPageList (void        *aTrans,
                                        scSpaceID    aSpaceID,
                                        UInt         aPageCount,
                                        void       **aHeadPage,
                                        void       **aTailPage);

    // �ϳ��� Page�� �����ͺ��̽��� �ݳ��Ѵ�
    static IDE_RC freePersPage (void     *   aTrans,
                                scSpaceID    aSpaceID,
                                void     *   aToBeFreePage );

    // �������� Page�� �Ѳ����� �����ͺ��̽��� �ݳ��Ѵ�.
    // aHeadPage���� aTailPage����
    // Page Header�� Prev/Next�����ͷ� ����Ǿ� �־�� �Ѵ�.
    static IDE_RC freePersPageList (void        *aTrans,
                                    scSpaceID    aSpaceID,
                                    void        *aHeadPage,
                                    void        *aTailPage);
    
    /* -----------------------
     * [*] retrieval of 
     *     the memory manager
     *     informations
     * ----------------------*/
    // Ư�� Page�� PCH�� �����´�.
    static svmPCH          * getPCH(scSpaceID aSpaceID, scPageID aPID );

    // OID�� �޸� �ּҰ����� ��ȯ�Ѵ�.
    static IDE_RC getOIDPtr(scSpaceID aSpaceID, smOID aOID, void ** aPtr);

    // Ư�� Page�� S��ġ�� ȹ���Ѵ�. ( ����� X��ġ�� �����Ǿ� �ִ� )
    static IDE_RC            holdPageSLatch(scSpaceID aSpaceID,
                                            scPageID  aPageID);
    // Ư�� Page�� X��ġ�� ȹ���Ѵ�. 
    static IDE_RC            holdPageXLatch(scSpaceID aSpaceID,
                                            scPageID  aPageID);
    // Ư�� Page���� ��ġ�� Ǯ���ش�.
    static IDE_RC            releasePageLatch(scSpaceID aSpaceID,
                                              scPageID  aPageID);

    static IDE_RC allocPageAlignedPtr(UInt a_nSize, void**, void**);

    static UInt   getDBMaxPageCount(svmTBSNode * aTBSNode)
    {
        return (UInt)aTBSNode->mDBMaxPageCount;
    }

    static void   getTableSpaceAttr(svmTBSNode * aTBSNode,
                                    smiTableSpaceAttr * aTBSAttr)
    {
        IDE_DASSERT( aTBSNode != NULL );
        IDE_DASSERT( aTBSAttr != NULL );
        
        idlOS::memcpy(aTBSAttr,
                      &aTBSNode->mTBSAttr,
                      ID_SIZEOF(smiTableSpaceAttr));
        aTBSAttr->mType = aTBSNode->mHeader.mType;
        aTBSAttr->mTBSStateOnLA = aTBSNode->mHeader.mState;
    }

    static void   getTBSAttrFlagPtr(svmTBSNode         * aTBSNode,
                                    UInt              ** aAttrFlagPtr)
    {
        IDE_DASSERT( aTBSNode != NULL );
        IDE_DASSERT( aAttrFlagPtr != NULL );

        *aAttrFlagPtr = &aTBSNode->mTBSAttr.mAttrFlag;
    }
    
    // �����ͺ��̽� ũ�⸦ �̿��Ͽ� �����ͺ��̽� Page���� ���Ѵ�.
    static ULong calculateDbPageCount( ULong aDbSize, ULong aChunkPageCount );

    // �ϳ��� Page�� �����Ͱ� ����Ǵ� �޸� ������ PCH Entry���� �����´�.
    static IDE_RC getPersPagePtr(scSpaceID    aSpaceID, 
                                 scPageID     aPID, 
                                 void      ** aPagePtr);

    // Space ID�� Valid�� ������ üũ�Ѵ�.
    static idBool isValidSpaceID( scSpaceID aSpaceID );

    // Page ID�� Valid�� ������ üũ�Ѵ�.
    static idBool isValidPageID( scSpaceID aSpaceID, scPageID aPageID );

    /***********************************************************
     * svmTableSpace ���� ȣ���ϴ� �Լ���
     ***********************************************************/
    // Tablespace���� ����� Page �޸� Ǯ�� �ʱ�ȭ�Ѵ�.
    static IDE_RC initializePagePool( svmTBSNode * aTBSNode );

    // Tablespace�� Meta Page�� �ʱ�ȭ�ϰ� Free Page���� �����Ѵ�.
    static IDE_RC createTBSPages( svmTBSNode      * aTBSNode,
                                  SChar           * aDBName,
                                  scPageID          aCreatePageCount);

};

/*
 * Ư�� Page�� PCH�� �����´�
 *
 * aPID [IN] PCH�� ������ �������� ID
 */
inline svmPCH *
svmManager::getPCH(scSpaceID aSpaceID, scPageID aPID )
{
    IDE_DASSERT( isValidSpaceID( aSpaceID ) == ID_TRUE );
    IDE_DASSERT( isValidPageID( aSpaceID, aPID ) == ID_TRUE );
    
    return mPCHArray[aSpaceID][ aPID ];
}

/*
 * OID�� �޸� �ּҰ����� ��ȯ�Ѵ�.
 *
 * aSpaceID - [IN]  TablespaceID
 * aOID     - [IN]  �޸� �ּҰ����� ��ȯ�� Object ID
 * aPtr     - [OUT] ����� Object �ּҰ�
 */
inline IDE_RC svmManager::getOIDPtr(scSpaceID aSpaceID, smOID aOID, void ** aPtr )
{
    scOffset sOffset;
    scPageID sPageID;

    sPageID = (scPageID)(aOID >> SM_OFFSET_BIT_SIZE);
    sOffset = (scOffset)(aOID & SM_OFFSET_MASK);

    IDE_TEST( getPersPagePtr( aSpaceID, sPageID, aPtr ) != IDE_SUCCESS );

    (*aPtr) = (void *)( ((UChar *)(*aPtr)) + sOffset  );

    return IDE_SUCCESS ;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

#if 0
/* TRUE�� �����ϰ� �־ �Լ�������. */

// Page ���� Valid�� ������ üũ�Ѵ�.
inline idBool
svmManager::isValidPageCount( svmTBSNode * /* aTBSNode*/,
                              vULong       /*aPageCount*/ )
{
/*
    IDE_ASSERT( mHighLimitPAGE > 0 );
    return ( aPageCount <= mHighLimitPAGE ) ?
             ID_TRUE : ID_FALSE ;
*/
    return ID_TRUE;
}
#endif

// Page ID�� Valid�� ������ üũ�Ѵ�.
inline idBool
svmManager::isValidSpaceID( scSpaceID aSpaceID )
{
    return (mPCHArray[aSpaceID] == NULL) ? ID_FALSE : ID_TRUE;
}

// Page ID�� Valid�� ������ üũ�Ѵ�.
inline idBool
svmManager::isValidPageID( scSpaceID aSpaceID ,
                           scPageID aPageID )
{
   svmTBSNode *sTBSNode;

    sTBSNode = (svmTBSNode*)sctTableSpaceMgr::getSpaceNodeBySpaceID(
        aSpaceID );

    if( sTBSNode != NULL )
    {
        IDE_DASSERT( sTBSNode->mDBMaxPageCount > 0 );

        return ( aPageID <= sTBSNode->mDBMaxPageCount ) ?
            ID_TRUE : ID_FALSE;

    }

    return ID_TRUE;
}


#endif // _O_SVM_MANAGER_H_