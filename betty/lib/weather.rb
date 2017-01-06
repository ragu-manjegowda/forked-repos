module Weather
  def self.interpret(command)
    responses = []

    if command.match(/^?(?:weather\s+)?like\s+(?:in\s+)?(.+)$/)
      search_term = $1.gsub(" ", "%20")

      responses << {
        :command => "curl -sS wttr.in/#{ search_term }".strip,
        :explanation => "Show weather for given place."
      }
    end

    responses
  end

  def self.help
    commands = []
    commands << {
      :category => "Weather",
      :description => "Show the weather forecast for the provided location",
      :usage => ["betty what is the weather like in London"]
    }
    commands
  end
end

$executors << Weather
