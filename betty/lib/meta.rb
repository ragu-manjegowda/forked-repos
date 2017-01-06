module Meta
  def self.interpret(command)
    responses = []

    if command.match(/^(what\s+)?version(\s+are\s+you)?$/)
      responses << {
        :say => $VERSION,
        :explanation => "Gets Betty's version."
      }
    end

    if command.match(/^whats?\s+(?:is\s+)?your\s+(website|url|github|repo)(\s+again\?)?$/) ||
       command.match(/^(website|url)$/)
      responses << {
        :say => $URL,
        :explanation => "Gets Betty's website."
      }
    end

    if command.match(/^what can you do for me$/)
      require 'io/console'

      topics = executors.flat_map(&:help)
                        .map {|executor| executor[:category] }
                        .sort

      responses << {
        say: "I'm happy to help you with all your needs.\n " \
             "Tell me if you want to know more about a topic, for example by asking: " \
             "'Betty help me with <topic>' or 'Betty I need help with <topic>'.\n\n" \
             "These are the topics I can help you with:\n" \
             "#{tableize topics}"
      }
    end

    responses
  end

  def self.help
    commands = []
    commands << {
      category: 'Meta',
      usage: [
        'what version are you (or just betty version)',
        'whats your github again',
        'what can you do for me'
      ]
    }
    commands
  end

  def self.executors
    $executors
  end

  def self.tableize(words)
    available_width = IO.console.winsize.last
    longest_word = words.max_by {|word| word.size }
    column_size = longest_word.size + 1
    column_count = available_width / column_size

    rows = if column_count < 2
      words
    else
      words.each_slice(column_count).map do |words_in_row|
        words_in_row.map { |word| word.ljust(column_size) }.join
      end
    end

    rows.join("\n")
  end
end

$executors << Meta
