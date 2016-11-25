
/*
 * Blob Class
 */
class Blob {
    
    private:
    
    int     m_nSize;
    int     m_nBlobId;
    int     m_nMemberId;
    long    m_lTimestamp;

    public:
    
    Blob(int size, int blobId, int memberId, long timestamp) {
        m_nSize      = size;
        m_nBlobId    = blobId;
        m_nMemberId  = memberId;
        m_lTimestamp = timestamp;
    }

    int getSize();
    void setSize(int m_nSize);
    int getBlob_id();
    int getMember_id();

};

int Blob::getSize(){
        return m_nSize;
}

void Blob::setSize(int m_nSize) {
        m_nSize = m_nSize;
}

int Blob::getBlob_id() {
        return m_nBlobId;
}

int Blob::getMember_id() {
        return m_nMemberId;
}    
