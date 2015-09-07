/**
 * \file nfcreaderunit.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC Reader unit.
 */

#ifndef LOGICALACCESS_NFCREADERUNIT_HPP
#define LOGICALACCESS_NFCREADERUNIT_HPP

#include "logicalaccess/readerproviders/readerunit.hpp"
#include "nfcreaderunitconfiguration.hpp"

#include "logicalaccess/logs.hpp"
#include "logicalaccess/myexception.hpp"

#include "nfc/nfc.h"
#include <map>

namespace logicalaccess
{
    class Profile;
	class NFCReaderCardAdapter;
    class NFCReaderProvider;

    /**
     * \brief The NFC reader unit class.
     */
    class LIBLOGICALACCESS_API NFCReaderUnit : public ReaderUnit
    {
    public:

        /**
         * \brief Constructor.
         */
		NFCReaderUnit(const std::string& name);

        /**
         * \brief Destructor.
         */
        virtual ~NFCReaderUnit();

        /**
         * \brief Get the reader unit name.
         * \return The reader unit name.
         */
        virtual std::string getName() const;

        /**
         * \brief Get the connected reader unit name.
         * \return The connected reader unit name.
         */
        virtual std::string getConnectedName();

        /**
         * \brief Set the card type.
         * \param cardType The card type.
         */
        virtual void setCardType(std::string cardType);

        /**
         * \brief Wait for a card insertion.
         * \param maxwait The maximum time to wait for, in milliseconds. If maxwait is zero, then the call never times out.
         * \return True if a card was inserted, false otherwise. If a card was inserted, the name of the reader on which the insertion was detected is accessible with getReader().
         * \warning If the card is already connected, then the method always fail.
         */
        virtual bool waitInsertion(unsigned int maxwait);

        /**
         * \brief Wait for a card removal.
         * \param maxwait The maximum time to wait for, in milliseconds. If maxwait is zero, then the call never times out.
         * \return True if a card was removed, false otherwise. If a card was removed, the name of the reader on which the removal was detected is accessible with getReader().
         */
        virtual bool waitRemoval(unsigned int maxwait);

        /**
         * \brief Create the chip object from card type.
         * \param type The card type.
         * \return The chip.
         */
        virtual std::shared_ptr<Chip> createChip(std::string type);

        virtual std::vector<unsigned char> getNumber(std::shared_ptr<Chip> chip) override;

/**
         * \brief Get the first and/or most accurate chip found.
         * \return The single chip.
         */
        virtual std::shared_ptr<Chip> getSingleChip();

        /**
         * \brief Get chip available in the RFID rang.
         * \return The chip list.
         */
        virtual std::vector<std::shared_ptr<Chip> > getChipList();

        /**
         * \brief Connect to the card.
         * \return True if the card was connected without error, false otherwise.
         *
         * If the card handle was already connected, connect() first call disconnect(). If you intend to do a reconnection, call reconnect() instead.
         */
        bool connect();

        /**
         * \brief Disconnect from the reader.
         * \see connect
         *
         * Calling this method on a disconnected reader has no effect.
         */
        void disconnect();

        /**
         * \brief Check if the card is connected.
         * \return True if the card is connected, false otherwise.
         */
        virtual bool isConnected();

        /**
         * \brief Connect to the reader. Implicit connection on first command sent.
         * \return True if the connection successed.
         */
        virtual bool connectToReader();

        /**
         * \brief Disconnect from reader.
         */
        virtual void disconnectFromReader();

        /**
         * \brief Get a string hexadecimal representation of the reader serial number
         * \return The reader serial number or an empty string on error.
         */
        virtual std::string getReaderSerialNumber();

		/**
		* \brief Get the default NFC reader/card adapter.
		* \return The default NFC reader/card adapter.
		*/
		virtual std::shared_ptr<NFCReaderCardAdapter> getDefaultNFCReaderCardAdapter();

        /**
         * \brief Serialize the current object to XML.
         * \param parentNode The parent node.
         */
        virtual void serialize(boost::property_tree::ptree& parentNode);

        /**
         * \brief UnSerialize a XML node to the current object.
         * \param node The XML node.
         */
        virtual void unSerialize(boost::property_tree::ptree& node);

        /**
         * \brief Get the NFC reader unit configuration.
         * \return The NFC reader unit configuration.
         */
        std::shared_ptr<NFCReaderUnitConfiguration> getNFCConfiguration() { return std::dynamic_pointer_cast<NFCReaderUnitConfiguration>(getConfiguration()); };

        /**
         * \brief Get the NFC reader provider.
         * \return The NFC reader provider.
         */
        std::shared_ptr<NFCReaderProvider> getNFCReaderProvider() const;

		static std::shared_ptr<NFCReaderUnit> createNFCReaderUnit(const std::string& readerName);

		/**
		* \brief Get the NFC device.
		* \return The NFC device.
		*/
		nfc_device* getDevice() const { return d_device; };

        void writeChipUid(std::shared_ptr<Chip> c, const std::vector<uint8_t> &new_uid);

        /**
         * Fetch the reader name by asking it.
         */
        std::string fetchRealName();

	protected:

        void refreshChipList();

        /**
        * Transmit bit using the NFC reader.
        * This API circumvent all the abstraction provided by reader card adapter and
        * data transport.
        */
        std::vector<uint8_t> transmitBits(const uint8_t *pbtTx, const size_t szTxBits);

		std::string getCardTypeFromTarget(nfc_target target);

		std::vector<unsigned char> getCardSerialNumber(nfc_target target);

		/**
		 * \brief The reader unit name.
		 */
		std::string d_name;

		/**
		 * \brief The reader unit connected name.
		 */
		std::string d_connectedName;

		bool d_chip_connected;

		/**
		 * \brief The NFC device.
		 */
		nfc_device *d_device;

		/**
		 * \brief Map liblogicalaccess Chip with libnfc objects
		 * \remarks This could be replaced by a context assigned to the chip but it would require LLA refactoring
		 */
		std::map<std::shared_ptr<Chip>, nfc_target> d_chips;

    private:
		/**
		 * Call a libnfc function and throw an exception is the return code is non zero.
		 */
		template<typename TargetFunction, typename ...Args>
		int nfc_safe_call(TargetFunction fct, Args... arg)
		{
			int ret = fct(arg...);
			if (ret != 0)
			{
				THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException, "NFC error: " + std::string(nfc_strerror(d_device)));
			}
			return ret;
		}

        /**
         * Change the NFC reader's config to perform rawer operation for changing the uid.
         * In the destructor, returns the configuration is more normal mode.
         */
        struct WriteUIDConfigGuard
        {
            WriteUIDConfigGuard(NFCReaderUnit &ru);
            ~WriteUIDConfigGuard();

        private:
            NFCReaderUnit &ru_;
            bool rca_error_flag_;
            bool dt_error_flag_;
        };
    };
}

#endif
