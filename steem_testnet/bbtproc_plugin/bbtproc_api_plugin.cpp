
#include <steemit/app/application.hpp>
#include <steemit/app/plugin.hpp>

#include <sstream>
#include <string>

namespace steemit { namespace example_plugin {

class bbtproc_api_plugin : public steemit::app::plugin
{
   public:
      /**
       * The plugin requires a constructor which takes app.  This is called regardless of whether the plugin is loaded.
       * The app parameter should be passed up to the superclass constructor.
       */
      bbtproc_api_plugin( steemit::app::application* app );

      /**
       * Plugin is destroyed via base class pointer, so a virtual destructor must be provided.
       */
      virtual ~bbtproc_api_plugin();

      /**
       * Every plugin needs a name.
       */
      virtual std::string plugin_name()const override;

      /**
       * Called when the plugin is enabled, but before the database has been created.
       */
      virtual void plugin_initialize( const boost::program_options::variables_map& options ) override;

      /**
       * Called when the plugin is enabled.
       */
      virtual void plugin_startup() override;

      std::string get_message();

   private:
      steemit::app::application* _app;
      std::string _message;
      uint32_t _plugin_call_count = 0;
};

class bbtproc_api_api
{
   public:
      bbtproc_api_api( const steemit::app::api_context& ctx );

      /**
       * Called immediately after the constructor.  If the API class uses enable_shared_from_this,
       * shared_from_this() is available in this method, which allows signal handlers to be registered
       * with app::connect_signal()
       */
      void on_api_startup();
      std::string get_message();

   private:
      steemit::app::application& _app;
      uint32_t _api_call_count = 0;
};

} }

FC_API( steemit::example_plugin::bbtproc_api_api,
   (get_message)
   )

namespace steemit { namespace example_plugin {

bbtproc_api_plugin::bbtproc_api_plugin( steemit::app::application* app ) : steemit::app::plugin(app) {}
bbtproc_api_plugin::~bbtproc_api_plugin() {}

std::string bbtproc_api_plugin::plugin_name()const
{
   return "bbtproc_api";
}

void bbtproc_api_plugin::plugin_initialize( const boost::program_options::variables_map& options )
{
   _message = "bbtproc world";
}

void bbtproc_api_plugin::plugin_startup()
{
   app().register_api_factory< bbtproc_api_api >( "bbtproc_api_api" );
}

std::string bbtproc_api_plugin::get_message()
{
   std::stringstream result;
   result << _message << " -- users have viewed this message " << (_plugin_call_count++) << " times";
   return result.str();
}

bbtproc_api_api::bbtproc_api_api( const steemit::app::api_context& ctx ) : _app(ctx.app) {}

void bbtproc_api_api::on_api_startup() {}

std::string bbtproc_api_api::get_message()
{
   std::stringstream result;
   std::shared_ptr< bbtproc_api_plugin > plug = _app.get_plugin< bbtproc_api_plugin >( "bbtproc_api" );
   result << plug->get_message() << " -- you've viewed this message " << (_api_call_count++) << " times";
   return result.str();
}

} }

/**
 * The STEEMIT_DEFINE_PLUGIN() macro will define a steemit::plugin::create_bbtproc_api_plugin()
 * factory method which is expected by the manifest.
 */

STEEMIT_DEFINE_PLUGIN( bbtproc_api, steemit::example_plugin::bbtproc_api_plugin )
