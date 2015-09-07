/**
 * \file nfcreaderprovider.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC card reader provider.
 */

#ifndef LOGICALACCESS_READERNFC_PROVIDER_HPP
#define LOGICALACCESS_READERNFC_PROVIDER_HPP

#include "logicalaccess/readerproviders/readerprovider.hpp"
#include "nfcreaderunit.hpp"

#include <string>
#include <vector>

#include "logicalaccess/logs.hpp"


namespace logicalaccess
{
#define READER_NFC		"NFC"

    /**
     * \brief NFC Reader Provider class.
     */
    class LIBLOGICALACCESS_API NFCReaderProvider : public ReaderProvider
    {
    public:

        /**
        * \brief Create a new NFC reader provider instance.
        * \return The NFC reader provider instance.
        */
        static std::shared_ptr<NFCReaderProvider> createInstance();

        /**
         * \brief Destructor.
         */
        ~NFCReaderProvider();

        /**
         * \brief Release the provider resources.
         */
        virtual void release();

        /**
         * \brief Get the reader provider type.
         * \return The reader provider type.
         */
        virtual std::string getRPType() const { return READER_NFC; };

        /**
         * \brief Get the reader provider name.
         * \return The reader provider name.
         */
        virtual std::string getRPName() const { return "NFC"; };

        /**
         * \brief List all readers of the system.
         * \return True if the list was updated, false otherwise.
         */
        virtual bool refreshReaderList();

        /**
         * \brief Get reader list for this reader provider.
         * \return The reader list.
         */
        virtual const ReaderList& getReaderList() { return d_readers; };

        /**
         * \brief Create a new reader unit for the reader provider.
         * param ip The reader address
         * param port The reader port
         * \return A reader unit.
         */
        virtual std::shared_ptr<ReaderUnit> createReaderUnit() override;

		/**
		* \brief Get the NFC context.
		* \return The NFC context.
		*/
		nfc_context* getContext() const { return d_context; };

    protected:

        /**
         * \brief Constructor.
         */
        NFCReaderProvider();

        /**
         * \brief The reader list.
         */
        ReaderList d_readers;

		/**
		 * \brief The NFC Context.
		*/
		nfc_context* d_context;
    };
}

#endif /* LOGICALACCESS_READERNFC_PROVIDER_HPP */