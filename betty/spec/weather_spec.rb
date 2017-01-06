require "spec_helper"

describe "Weather" do

  context "weather like in San Francisco" do
    it { responds_with command: "curl -sS wttr.in/San%20Francisco", :explanation => "Show weather for given place." }
  end

end
